#!/usr/bin/bash 

rm -f log.txt
rm -f work.txt.lck

./locker & sleep 1
echo 0 > work.txt.lck
sleep 1

echo "We have log:"
cat log.txt
