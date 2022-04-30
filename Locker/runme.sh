#!/usr/bin/bash 

rm -f log.txt
rm -f work.txt.lck

for (( i = 1; i <= 10; i++ )) do ./locker & sleep 1; done
sleep 60

killall -SIGINT locker
sleep 1

echo "We have log:"
cat log.txt
