#!/system/bin/sh
# Bring-up logging (init.ef44.debug.rc): every 30 seconds, rewrite the
# kernel log on /cache when it changed and append temperatures, CPU clocks
# and the governor, then sync /cache only, so the logs survive the phone
# losing power. (It used to rewrite ~128 KB every 5 s, which cost flash
# wear and battery for little gain once the port was stable.)
D=/cache/ef44log/cur
last=""
while true; do
    sig=$(dmesg | tail -1)
    if [ "$sig" != "$last" ]; then
        dmesg > $D/dmesg.tmp && mv -f $D/dmesg.tmp $D/dmesg.txt
        last=$sig
    fi
    echo "$(cat /proc/uptime) $(cat /sys/class/thermal/thermal_zone0/temp)" \
        "$(cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_cur_freq 2>/dev/null | tr '\n' ' ')" \
        "$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor)" >> $D/temps.txt
    sync -f $D/dmesg.txt
    sleep 30
done
