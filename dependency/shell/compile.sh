#! /bin/bash 
# WORKDIR /usr/local/PlayerCode/CAPI/cpp
i=1
flag=1
bind=/usr/local/mnt
while (( $i <= 5 ))
do
    if [ -f "${bind}/player${i}.cpp" ]; then
        cp -f $bind/player$i.cpp ./API/src/AI.cpp
        cmake ./CMakeLists.txt && make -j$(nproc) >compile_log$i.txt 2>&1
        mv ./capi $bind/capi$i # executable file
        if [ $? -ne 0 ]; then
            flag=0
        fi
        mv ./compile_log$i.txt $bind/compile_log$i.txt
    elif [ -f "${bind}/player${i}.py" ]; then
        pushd ../python
        cp -f $bind/player$i.py ./PyAPI/AI.py
        python3 -m compileall ./PyAPI >compile_log$i.txt 2>&1
        if [ ! -f ./PyAPI/__pycache__/AI.cpython-39.pyc ]; then
            flag=0
        else
            rm -rf ./PyAPI/__pycache__/AI.cpython-39.pyc
        fi
        mv ./compile_log$i.txt $bind/compile_log$i.txt
        popd
    fi
    let "i++"
done
# HTML request to update status.
if [ $flag -eq 1 ]; then
    curl $URL -X PUT -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d '{"compile_status":"compiled"}' > $bind/curl_log.txt
else
    curl $URL -X PUT -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d '{"compile_status":"failed"}' > $bind/curl_log.txt
fi
