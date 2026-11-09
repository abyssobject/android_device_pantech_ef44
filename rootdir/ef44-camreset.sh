#!/system/bin/sh
# Bring the camera stack up in order once boot has completed.
#
# The JB mm-qcamera-daemon must be fully initialised before the camera HAL
# talks to it. At boot, cameraserver probes the cameras (and the flash unit
# for the torch) while the daemon is still starting; the daemon's
# post-processing thread then stops acknowledging commands and the camera
# stays wedged (no preview, "Camera 0 has no flashlight"). Restarting the
# three pieces in order (daemon, wait, HAL, cameraserver) gives a clean
# state and a correct flash-unit probe.

stop cameraserver
stop vendor.camera-provider-2-4
stop qcamerasvr
sleep 1

start qcamerasvr
# the daemon needs a few seconds to load its libraries and open its socket
sleep 4

start vendor.camera-provider-2-4
sleep 2
start cameraserver
