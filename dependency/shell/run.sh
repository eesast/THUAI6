#!/usr/local

nice -10 ./Server/Server --port 8888 --studentCount 4 --trickerCount 1 --gameTimeInSecond 600
sleep 5
cd /usr/local/thuai6

for i in {1..5}
do
    j=$((i - 1))
    if [-f "./python/player$i.py"]; then
        nice -0 python3 ./python/player$i.py -I 127.0.0.1 -P 8888 -p $j
    elif [-f "./capi$i"]; then
        nice -0 ./capi$i -I 127.0.0.1 -P 8888 -p $j
    else
        echo "ERROR. $i is not found."
    fi
done
