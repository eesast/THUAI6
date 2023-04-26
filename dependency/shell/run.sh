#!/usr/local

python_dir=/usr/local/PlayerCode/CAPI/python/PyAPI
playback_dir=/usr/local/playback

nice -10 ./Server --port 8888 --studentCount 4 --trickerCount 1 --gameTimeInSecond 600 --url $URL --token $TOKEN --fileName $playback_dir/video --startLockFile $playback_dir/start.lock > $playback_dir/server.log &
server_pid=$!
sleep 5
for k in {1..2}
do
    pushd /usr/local/team$k
    if [ $k -eq 1 ]; then
        for i in {1..4}
        do
            j=$((i - 1))
            if [ -f "./player$i.py" ]; then
                cp -f ./player$i.py $python_dir/AI.py
                nice -0 python3 $python_dir/main.py -I 127.0.0.1 -P 8888 -p $j > $playback_dir/team$k-player$j.log &
            elif [ -f "./capi$i" ]; then
                nice -0 ./capi$i -I 127.0.0.1 -P 8888 -p $j > $playback_dir/team$k-player$j.log &
            else
                echo "ERROR. $i is not found."
            fi
        done
    else
        for i in {5..5}
        do
            j=$((i - 1))
            if [ -f "./player$i.py" ]; then
                cp -f ./player$i.py $python_dir/AI.py
                nice -0 python3 $python_dir/main.py -I 127.0.0.1 -P 8888 -p $j > $playback_dir/team$k-player$j.log &
            elif [ -f "./capi$i" ]; then
                nice -0 ./capi$i -I 127.0.0.1 -P 8888 -p $j > $playback_dir/team$k-player$j.log &
            else
                echo "ERROR. $i is not found."
            fi
        done
    fi
    popd
done

sleep 10

if [ -f $playback_dir/start.lock ]; then
    ps -p $server_pid
    while [ $? -eq 0 ]
    do
        sleep 1
        ps -p $server_pid
    done
    touch $playback_dir/finish.lock
    echo "Finish"
else
    echo "Failed to start game."
    touch temp.lock
    mv -f temp.lock $playback_dir/video.thuaipb
    kill -9 $server_pid
fi
