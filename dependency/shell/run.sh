#!/usr/local
python_dir=/usr/local/PlayerCode/CAPI/python/PyAPI
python_main_dir=/usr/local/PlayerCode/CAPI/python
playback_dir=/usr/local/output
map_dir=/usr/local/map
mkdir -p $playback_dir

# initialize
if [[ "${MODE}" == "ARENA" ]]; then
    MODE_NUM=0
elif [[ "${MODE}" == "COMPETITION" ]]; then
    MODE_NUM=1
fi

# set default value
: "${TEAM_SEQ_ID:=0}"
: "${TEAM_LABELS:=Student:Tricker}"
: "${TEAM_LABEL:=Student}"
: "${EXPOSED=1}"
: "${MODE_NUM=0}"
: "${GAME_TIME=10}"
: "${CONNECT_IP=172.17.0.1}"


get_current_team_label() {
    if [ $TEAM_SEQ_ID -eq $2 ]; then
        echo "find current team label: $1"
        current_team_label=$1
    fi
}

read_array() {
    callback=$1
    echo "read array: set callback command: $callback"

    IFS=':' read -r -a fields <<< "$2"

    count=0 # loop count

    for field in "${fields[@]}"
    do
        echo "parse field: $field"
        param0=$field

        # call command
        run_command="$callback $param0 $count"
        echo "Call Command: $run_command"
        $run_command

        count=$((count+1))
    done
}


function retry_command {
    local command="$1"
    local max_attempts=2
    local attempt_num=1
    local sleep_seconds=10

    while [ $attempt_num -le $max_attempts ]; do
        echo "Attempt $attempt_num / $max_attempts to run command: $command"

        eval $command &
        local PID=$!

        sleep $sleep_seconds

        if kill -0 $PID 2>/dev/null; then
            echo "Connected to server successfully."
            return 0
        else
            echo "Failed to connect to server. Retrying..."
            ((attempt_num++))
        fi
    done

    echo "Failed to connect to server after $max_attempts attempts."
    return 1
}



if [ "$TERMINAL" = "SERVER" ]; then
    map_path=$map_dir/$MAP_ID.txt
    # allow spectator always.
    nice -10 ./Server --port 8888 --studentCount 4 --trickerCount 1 --resultFileName $playback_dir/result --gameTimeInSecond $GAME_TIME --mode $MODE_NUM --mapResource $map_path --url $SCORE_URL --token $TOKEN --fileName $playback_dir/video --startLockFile $playback_dir/start.lock > $playback_dir/server.log 2>&1 &
    server_pid=$!
    echo "server pid: $server_pid"
    ls $playback_dir

    echo "SCORE URL: $SCORE_URL"
    echo "FINISH URL: $FINISH_URL"

    echo "waiting..."
    sleep 30 # wait connection time
    echo "watching..."

    if [ -f $playback_dir/start.lock ]; then
        ps -p $server_pid
        while [ $? -eq 0 ]
        do
            sleep 1
            ps -p $server_pid > /dev/null 2>&1
        done

        # update score by finish url

        echo "Getting result score..."
        result=$(cat $playback_dir/result.json)
        score0=$(echo "$result" | grep -oP '(?<="Student":)\d+') # Student score
        score1=$(echo "$result" | grep -oP '(?<="Tricker":)\d+') # Tricker score
        echo "Result score: Student: $score0, Tricker: $score1"

        # detect two team seqs:
        echo "Parsing TEAM_LABELS: $TEAM_LABELS"
        TEAM_SEQ_ID=0
        read_array get_current_team_label $TEAM_LABELS
        if [[ "${current_team_label}" == "Student" ]]; then
            echo "Parse Success: 1st team is Student"
            finish_payload='{"result": {"status": "Finished", "scores": ['${score0}', '${score1}']}}'
        elif [[ "${current_team_label}" == "Tricker" ]]; then
            echo "Parse Success: 1st team is Tricker"
            finish_payload='{"result": {"status": "Finished", "scores": ['${score1}', '${score0}']}}'
        else
            echo "Parse Failure: 1st team is Unknown"
            finish_payload='{"result": {"status": "Crashed", "scores": [0, 0]}}'
        fi

        if [[ -n $finish_payload ]]; then
            echo "FINISH_URL: $FINISH_URL, payload: $finish_payload. Start update score..."
            curl $FINISH_URL -X POST -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d "${finish_payload}" > $playback_dir/send.log 2>&1
        else
            echo "Payload not set."
        fi

        # Congratulations! You have finished the competition!!!!!
        # touch $playback_dir/finish.lock
        echo "Finish!"
    else
        echo "Failed to start game."
        touch $playback_dir/finish.lock
        touch temp.lock
        mv -f temp.lock $playback_dir/video.thuaipb
        kill -9 $server_pid
        finish_payload='{"result": {"status": "Crashed", "scores": [0, 0]}}'
        curl $FINISH_URL -X POST -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d "${finish_payload}" > $playback_dir/send.log 2>&1
    fi


elif [ "$TERMINAL" = "CLIENT" ]; then
    echo "Client Mode! Team Label data - $TEAM_LABEL"

    # parse team label name
    current_team_label=$TEAM_LABEL

    # k is an enum (1,2), 1 = Student, 2 = Tricker
    if [ "$current_team_label" = "Student" ]; then
        k=1
    elif [ "$current_team_label" = "Tricker" ]; then
        k=2
    else
        echo "Error: Invalid Team Label"
        exit
    fi
    pushd /usr/local/code
        if [ $k -eq 1 ]; then
            for i in {1..4}
            do
                j=$((i - 1)) # student player id from 0 to 3
                code_name=Student$i
                if [ -f "./$code_name.py" ]; then
                    echo "find ./$code_name.py"
                    cp -r $python_main_dir $python_main_dir$i
                    cp -f ./$code_name.py $python_main_dir$i/PyAPI/AI.py

                    command="nice -0 python3 $python_main_dir$i/PyAPI/main.py -I $CONNECT_IP -P $PORT -p $j > $playback_dir/team$k-player$j.log 2>&1"

                    retry_command "$command" &

                    ps -aux |grep main.py

                elif [ -f "./$code_name" ]; then
                    echo "find ./$code_name"

                    command="nice -0 ./$code_name -I $CONNECT_IP -P $PORT -p $j > $playback_dir/team$k-player$j.log 2>&1"

                    ps -aux |grep $code_name

                else
                    echo "ERROR. $code_name is not found."
                fi
            done
        else
            i=5
            j=4 # tricker id is 4
            code_name=Tricker
            if [ -f "./$code_name.py" ]; then
                echo "find ./$code_name.py"
                cp -r $python_main_dir $python_main_dir$i
                cp -f ./$code_name.py $python_main_dir$i/PyAPI/AI.py

                command="nice -0 python3 $python_main_dir$i/PyAPI/main.py -I $CONNECT_IP -P $PORT -p $j > $playback_dir/team$k-player$j.log 2>&1"

                retry_command "$command"

                ps -aux |grep main.py
            elif [ -f "./$code_name" ]; then
                echo "find ./$code_name"

                command="nice -0 ./$code_name -I $CONNECT_IP -P $PORT -p $j > $playback_dir/team$k-player$j.log 2>&1"

                retry_command "$command"

                ps -aux |grep $code_name
            else
                echo "ERROR. $code_name is not found."
            fi
        fi

        # curl $CONNECT_IP:$PORT

        sleep $((GAME_TIME * 2))

    popd
else
    echo "VALUE ERROR: TERMINAL is neither SERVER nor CLIENT."
fi
