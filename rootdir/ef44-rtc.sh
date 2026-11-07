#!/system/bin/sh
# Keep the wall clock across reboots.
#
# The PM8921 RTC is read-only for Android on this board (Qualcomm used
# time_daemon with offsets in /data/time, which needs their framework
# service), so after every boot the clock fell back to an old time until
# network time arrived. Keep "wall clock - RTC" in /data and add it back to
# the RTC at boot.
#
#   ef44-rtc.sh restore   run once from post-fs-data
#   ef44-rtc.sh save      loop: store the offset whenever it changes
#   ef44-rtc.sh saveonce  store it now (shutdown)

RTC=/sys/class/rtc/rtc0/since_epoch
F=/data/vendor/ef44/rtc_offset
MIN_SANE=1767225600 # 2026-01-01: anything earlier is not a real clock

case "$1" in
restore)
    [ -r $F ] || exit 0
    off=$(cat $F); rtc=$(cat $RTC)
    [ -n "$off" ] && [ -n "$rtc" ] || exit 0
    t=$((rtc + off)); now=$(date +%s)
    # only move the clock forward; never undo a newer network time
    if [ $t -gt $((now + 60)) ]; then
        date @$t > /dev/null
        log -t ef44-rtc "clock restored from RTC + offset: $(date)"
    fi
    ;;
saveonce)
    now=$(date +%s); rtc=$(cat $RTC)
    [ $now -gt $MIN_SANE ] && [ -n "$rtc" ] && echo $((now - rtc)) > $F
    ;;
save)
    last=$(cat $F 2>/dev/null)
    while true; do
        now=$(date +%s); rtc=$(cat $RTC)
        if [ $now -gt $MIN_SANE ] && [ -n "$rtc" ]; then
            off=$((now - rtc))
            # write only when the offset moved by more than 2 s
            if [ -z "$last" ] || [ $((off - last)) -gt 2 ] || [ $((last - off)) -gt 2 ]; then
                echo $off > $F.tmp && mv -f $F.tmp $F
                last=$off
            fi
        fi
        sleep 60
    done
    ;;
esac
