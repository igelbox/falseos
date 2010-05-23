#!/bin/bash
ant=/usr/local/netbeans-6.8/java3/ant/bin/ant
if [ -z "$1" ]; then
    $ant run
else
    $ant -f $1 run
fi
