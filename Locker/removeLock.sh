#!/usr/bin/bash 

rm -f log.txt
rm -f work.txt.lck

./locker & sleep 1
rm -f work.txt.lck
sleep 1

echo "We have log:"
cat log.txt
