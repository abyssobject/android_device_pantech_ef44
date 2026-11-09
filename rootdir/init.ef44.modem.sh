#!/system/bin/sh
# Fallback only. On about half the boots right after TWRP the modem's first
# remote storage request was never answered, so the modem never finished
# booting: no QMI services, rild crashing on NULL clients every 5 s. Two
# bugs, both fixed (2026-11-06):
#  - the kernel's IPC router socket ignored O_NONBLOCK, so rmt_storage
#    (QCSI) blocked on its empty control port (kernel b814cfc2030);
#  - rmt_storage passes QCSI's own fd set to select(), so its server socket
#    dropped out when the control port woke it alone (libshim_rmt
#    rmt_select.c).
# This script stays as a safety net.
#
# Restarting rmt_storage makes the modem restart (SSR, restart_level=3). That
# usually brings it up, but it has also ended in repeated modem restarts and
# a SoC reset (2026-11-06).
# rmt_storage maps the EFS buffer from /dev/mem only once the modem has
# connected, so it is never restarted while the modem is using it.
#
# rild starts with class main, usually before the modem's QMI services are
# up; qcril then fails its DMS/UIM init and never retries (no baseband, no
# SIM state). Once the modem is connected, restart rild once if that
# happened.

modem_connected() {
    pid=$(pidof rmt_storage) || return 1
    grep -q /dev/mem /proc/$pid/maps
}

fix_ril() {
    sleep 5
    if [ -z "$(getprop gsm.version.baseband)" ]; then
        log -p w -t ef44-modem "qcril started before the modem was ready, restarting rild"
        setprop ctl.restart vendor.ril-daemon
    fi
}

for i in 1 2; do
    sleep 30
    pidof rmt_storage > /dev/null || exit 0
    if modem_connected; then
        fix_ril
        exit 0
    fi
    log -p w -t ef44-modem "modem never reached rmt_storage, restarting it ($i)"
    setprop ctl.restart rmt_storage
    sleep 30
done
modem_connected && fix_ril
