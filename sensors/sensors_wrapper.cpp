/*
 * sensors.ef44: presents Pantech's sensors.msm8960.so blob, a version 0.1
 * sensors HAL from 2013, as the SENSORS_DEVICE_API_VERSION_1_3 HAL that
 * android.hardware.sensors@1.0-impl requires.
 *
 * - The sensor list is copied into current sensor_t structs (the fields up to
 *   minDelay have the same layout), with type strings, maxDelay and reporting
 *   flags filled in.
 * - batch() becomes setDelay(); the blob has no FIFO.
 * - flush() is answered by the wrapper: a META_DATA_FLUSH_COMPLETE event is
 *   queued and returned by the next poll().
 *
 * Selected with ro.hardware.sensors=ef44.
 */

#define LOG_TAG "sensors.ef44"

#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/hardware.h>
#include <hardware/sensors.h>
#include <log/log.h>

#define BLOB_PATH "/system/lib/hw/sensors.msm8960.so"
#define MAX_PENDING_FLUSHES 32

/* sensor_t as the 0.1 HAL knew it, up to the fields we use */
struct legacy_sensor_t {
    const char *name;
    const char *vendor;
    int version;
    int handle;
    int type;
    float maxRange;
    float resolution;
    float power;
    int32_t minDelay;
};

static struct sensors_module_t *blob_module;
static struct sensors_poll_device_t *blob_dev;
static struct sensor_t *sensor_list;
static int sensor_count = -1;

static pthread_mutex_t flush_lock = PTHREAD_MUTEX_INITIALIZER;
static int pending_flushes[MAX_PENDING_FLUSHES];
static int pending_flush_count;

static int load_blob(void)
{
    void *handle;

    if (blob_module)
        return 0;
    handle = dlopen(BLOB_PATH, RTLD_NOW);
    if (!handle) {
        ALOGE("dlopen %s: %s", BLOB_PATH, dlerror());
        return -EINVAL;
    }
    blob_module = (struct sensors_module_t *)dlsym(handle, HAL_MODULE_INFO_SYM_AS_STR);
    if (!blob_module) {
        ALOGE("no %s in %s", HAL_MODULE_INFO_SYM_AS_STR, BLOB_PATH);
        dlclose(handle);
        return -EINVAL;
    }
    return 0;
}

static const char *string_type(int type)
{
    switch (type) {
    case SENSOR_TYPE_ACCELEROMETER: return SENSOR_STRING_TYPE_ACCELEROMETER;
    case SENSOR_TYPE_MAGNETIC_FIELD: return SENSOR_STRING_TYPE_MAGNETIC_FIELD;
    case SENSOR_TYPE_ORIENTATION: return SENSOR_STRING_TYPE_ORIENTATION;
    case SENSOR_TYPE_GYROSCOPE: return SENSOR_STRING_TYPE_GYROSCOPE;
    case SENSOR_TYPE_LIGHT: return SENSOR_STRING_TYPE_LIGHT;
    case SENSOR_TYPE_PRESSURE: return SENSOR_STRING_TYPE_PRESSURE;
    case SENSOR_TYPE_PROXIMITY: return SENSOR_STRING_TYPE_PROXIMITY;
    case SENSOR_TYPE_GRAVITY: return SENSOR_STRING_TYPE_GRAVITY;
    case SENSOR_TYPE_LINEAR_ACCELERATION: return SENSOR_STRING_TYPE_LINEAR_ACCELERATION;
    case SENSOR_TYPE_ROTATION_VECTOR: return SENSOR_STRING_TYPE_ROTATION_VECTOR;
    default: return "";
    }
}

static uint32_t reporting_flags(int type)
{
    switch (type) {
    case SENSOR_TYPE_PROXIMITY:
        return SENSOR_FLAG_ON_CHANGE_MODE | SENSOR_FLAG_WAKE_UP;
    case SENSOR_TYPE_LIGHT:
        return SENSOR_FLAG_ON_CHANGE_MODE;
    default:
        return SENSOR_FLAG_CONTINUOUS_MODE;
    }
}

/* Types the 0.1 HAL era defined (accelerometer .. rotation vector). The
 * blob's higher numbers are Qualcomm-private and collide with newer AOSP
 * types, so those sensors are not exposed. */
static bool is_standard_type(int type)
{
    return type >= SENSOR_TYPE_ACCELEROMETER && type <= SENSOR_TYPE_ROTATION_VECTOR;
}

#define MAX_HIDDEN 16
static int hidden_handles[MAX_HIDDEN];
static int hidden_count;

static void mark_hidden(int handle)
{
    if (hidden_count < MAX_HIDDEN)
        hidden_handles[hidden_count++] = handle;
}

static bool is_hidden(int handle)
{
    for (int i = 0; i < hidden_count; i++)
        if (hidden_handles[i] == handle)
            return true;
    return false;
}

static int wrapper_get_sensors_list(struct sensors_module_t *module __unused,
                                    struct sensor_t const **list)
{
    const struct legacy_sensor_t *legacy;
    const struct sensor_t *blob_list;
    size_t legacy_size;
    int i, n;

    if (sensor_count >= 0) {
        *list = sensor_list;
        return sensor_count;
    }
    if (load_blob()) {
        *list = NULL;
        return 0;
    }
    n = blob_module->get_sensors_list(blob_module, &blob_list);
    if (n <= 0) {
        *list = NULL;
        return 0;
    }
    /* the 0.1 sensor_t had reserved words after minDelay instead of the
     * newer fields; it is the same size, so walk it with the current stride */
    legacy_size = sizeof(struct sensor_t);
    sensor_list = (struct sensor_t *)calloc(n, sizeof(struct sensor_t));
    int out = 0;
    for (i = 0; i < n; i++) {
        legacy = (const struct legacy_sensor_t *)((const char *)blob_list + i * legacy_size);
        if (!is_standard_type(legacy->type)) {
            /* Qualcomm-private types (Basic Gestures, Gyro Tap, Facing, Tilt)
             * reuse numbers that now mean uncalibrated magnetometer, game
             * rotation vector, uncalibrated gyroscope, significant motion */
            ALOGI("sensor %d: %s (type %d, handle %d) hidden", i, legacy->name,
                  legacy->type, legacy->handle);
            mark_hidden(legacy->handle);
            continue;
        }
        sensor_list[out].name = legacy->name;
        sensor_list[out].vendor = legacy->vendor;
        sensor_list[out].version = legacy->version;
        sensor_list[out].handle = legacy->handle;
        sensor_list[out].type = legacy->type;
        sensor_list[out].maxRange = legacy->maxRange;
        sensor_list[out].resolution = legacy->resolution;
        sensor_list[out].power = legacy->power;
        sensor_list[out].minDelay = legacy->minDelay;
        sensor_list[out].stringType = string_type(legacy->type);
        sensor_list[out].requiredPermission = "";
        sensor_list[out].maxDelay = 1000000;
        sensor_list[out].flags = reporting_flags(legacy->type);
        if (sensor_list[out].flags & SENSOR_FLAG_ON_CHANGE_MODE)
            sensor_list[out].minDelay = 0;
        ALOGI("sensor %d: %s (type %d, handle %d)", i, legacy->name, legacy->type,
              legacy->handle);
        out++;
    }
    sensor_count = out;
    *list = sensor_list;
    return out;
}

static int wrapper_activate(struct sensors_poll_device_t *dev __unused, int handle, int enabled)
{
    return blob_dev->activate(blob_dev, handle, enabled);
}

static int wrapper_set_delay(struct sensors_poll_device_t *dev __unused, int handle, int64_t ns)
{
    return blob_dev->setDelay(blob_dev, handle, ns);
}

static int wrapper_batch(struct sensors_poll_device_1 *dev __unused, int handle, int flags __unused,
                         int64_t period_ns, int64_t timeout __unused)
{
    return blob_dev->setDelay(blob_dev, handle, period_ns);
}

static int wrapper_flush(struct sensors_poll_device_1 *dev __unused, int handle)
{
    int ret = 0;

    pthread_mutex_lock(&flush_lock);
    if (pending_flush_count < MAX_PENDING_FLUSHES)
        pending_flushes[pending_flush_count++] = handle;
    else
        ret = -ENOMEM;
    pthread_mutex_unlock(&flush_lock);
    return ret;
}

static int wrapper_poll(struct sensors_poll_device_t *dev __unused, sensors_event_t *data,
                        int count)
{
    int n = 0;

    pthread_mutex_lock(&flush_lock);
    while (pending_flush_count > 0 && n < count) {
        memset(&data[n], 0, sizeof(data[n]));
        data[n].version = META_DATA_VERSION;
        data[n].type = SENSOR_TYPE_META_DATA;
        data[n].meta_data.what = META_DATA_FLUSH_COMPLETE;
        data[n].meta_data.sensor = pending_flushes[--pending_flush_count];
        n++;
    }
    pthread_mutex_unlock(&flush_lock);
    if (n > 0)
        return n;

    for (;;) {
        int got = blob_dev->poll(blob_dev, data, count);
        int keep = 0;

        if (got <= 0 || hidden_count == 0)
            return got;
        /* drop events from sensors we did not expose */
        for (int i = 0; i < got; i++) {
            if (is_hidden(data[i].sensor))
                continue;
            if (keep != i)
                data[keep] = data[i];
            keep++;
        }
        if (keep > 0)
            return keep;
    }
}

static int wrapper_close(struct hw_device_t *dev)
{
    int ret = blob_dev->common.close(&blob_dev->common);

    free(dev);
    return ret;
}

static int wrapper_open(const struct hw_module_t *module, const char *id, struct hw_device_t **device)
{
    struct sensors_poll_device_1 *dev;
    int ret;

    if (strcmp(id, SENSORS_HARDWARE_POLL))
        return -EINVAL;
    if ((ret = load_blob()))
        return ret;
    ret = blob_module->common.methods->open(&blob_module->common, id,
                                            (struct hw_device_t **)&blob_dev);
    if (ret) {
        ALOGE("blob open failed: %d", ret);
        return ret;
    }
    ALOGI("blob device version 0x%x, presenting 1.3", blob_dev->common.version);

    dev = (struct sensors_poll_device_1 *)calloc(1, sizeof(*dev));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = SENSORS_DEVICE_API_VERSION_1_3;
    dev->common.module = (struct hw_module_t *)module;
    dev->common.close = wrapper_close;
    dev->activate = wrapper_activate;
    dev->setDelay = wrapper_set_delay;
    dev->poll = wrapper_poll;
    dev->batch = wrapper_batch;
    dev->flush = wrapper_flush;
    *device = &dev->common;
    return 0;
}

static struct hw_module_methods_t wrapper_module_methods = {
    .open = wrapper_open,
};

__attribute__((visibility("default")))
struct sensors_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = SENSORS_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = SENSORS_HARDWARE_MODULE_ID,
        .name = "ef44 sensors wrapper",
        .author = "ef44",
        .methods = &wrapper_module_methods,
    },
    .get_sensors_list = wrapper_get_sensors_list,
};
