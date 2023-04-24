#!/usr/local

nice -10 ./Server/Server --port 8888 --studentCount 4 --trickerCount 1 --gameTimeInSecond 600 --url $URL --token $TOKEN
sleep 5
for k in {1..2}
do
    pushd /usr/local/team$k
    if [ $k -eq 1 ]; then
        for i in {1..4}
        do
            j=$((i - 1))
            if [ -f "./python/player$i.py" ]; then
                nice -0 python3 ./python/player$i.py -I 127.0.0.1 -P 8888 -p $j
            elif [ -f "./capi$i" ]; then
                nice -0 ./capi$i -I 127.0.0.1 -P 8888 -p $j
            else
                echo "ERROR. $i is not found."
            fi
        done
    else
        for i in {5..5}
        do
            j=$((i - 1))
            if [ -f "./python/player$i.py" ]; then
                nice -0 python3 ./python/player$i.py -I 127.0.0.1 -P 8888 -p $j
            elif [ -f "./capi$i" ]; then
                nice -0 ./capi$i -I 127.0.0.1 -P 8888 -p $j
            else
                echo "ERROR. $i is not found."
            fi
        done
    fi
    popd
done
