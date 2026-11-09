#!/system/bin/sh
# Set the Bluetooth address from the factory NV copy in /persist.
#
# The Broadcom vendor library reads persist.service.bdroid.bdaddr; without it
# the BT HAL gives up ("No Bluetooth Address!"). The stock firmware keeps the
# address in /persist/.bt_nv.bin as btnvtool data: 01 00 06 followed by the
# six address bytes, least significant first.

[ -n "$(getprop persist.service.bdroid.bdaddr)" ] && exit 0

set -- $(od -An -tx1 -N9 /persist/.bt_nv.bin 2>/dev/null)
if [ "$1" = "01" ] && [ "$3" = "06" ] && [ -n "$9" ]; then
    setprop persist.service.bdroid.bdaddr \
        "$(echo "$9:$8:$7:$6:$5:$4" | tr 'a-f' 'A-F')"
fi
