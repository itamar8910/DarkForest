#! /bin/sh
# Script to bring a network (tap) device for qemu up.
# The idea is to add the tap device to the same bridge
# as we have default routing to.

# in order to be able to find brctl
PATH=$PATH:/sbin:/usr/sbin
ip=$(which ip)

if [ -n "$ip" ]; then
   ip link set "$1" up
else
   brctl=$(which brctl)
   if [ ! "$ip" -o ! "$brctl" ]; then
     echo "W: $0: not doing any bridge processing: neither ip nor brctl utility not found" >&2
     exit 0
   fi
   ifconfig "$1" 0.0.0.0 up
fi

ifconfig "$1" 192.168.2.1 up


switch=$(ip route ls | \
    awk '/^default / {
          for(i=0;i<NF;i++) { if ($i == "dev") { print $(i+1); next; } }
         }'
        )

# only add the interface to default-route bridge if we
# have such interface (with default route) and if that
# interface is actually a bridge.
# It is possible to have several default routes too
for br in $switch; do
    if [ -d /sys/class/net/$br/bridge/. ]; then
        if [ -n "$ip" ]; then
          ip link set "$1" master "$br"
        else
          brctl addif $br "$1"
        fi
        exit	# exit with status of the previous command
    fi
done

echo "W: $0: no bridge for guest interface found" >&2
