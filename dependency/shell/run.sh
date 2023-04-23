#!/usr/local

nice -10 ./Server/Server --port 8888 --studentCount 4 --trickerCount 1 --gameTimeInSecond 600
sleep 5
for k in {1..2}
do
    pushd /usr/local/team$k
    for i in {1..5}
    do
        if [-f "./capi$i"]; then
            j=$((i - 1))
            nice -0 ./capi$i -I 127.0.0.1 -P 8888 -p $j
        elif [-f "./python/player$i.py"]; then
            nice -0 python3 ./python/player$i.py -I 127.0.0.1 -P 8888 -p $j
        fi
    done
    popd
done
