#!/bin/sh -x
PASS=pass
IP=c0

die() {
    echo "usage: $0 PID";
    exit 1
}

[ "$1" ] || die


while true; do
    sshpass -p $PASS ssh $IP kill -39 $1
    sleep 10
    sshpass -p $PASS scp $IP:/tmp/dmm_\* .
    sshpass -p $PASS ssh $IP rm -f /tmp/dmm_\*
    sleep 3590
done
