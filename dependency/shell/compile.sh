#! /bin/bash 
# WORKDIR /usr/local/PlayerCode/CAPI/cpp
workdir=/usr/local/PlayerCode/CAPI/cpp
bind=/usr/local/code
output=/usr/local/output

flag=1

cd $bind
file_count=$(ls -l *.cpp | wc -l);
if [ $file_count -eq 1 ]
then
    filename=$(ls *.cpp)
    cd $workdir
    cp -f $bind/$filename $workdir/API/src/AI.cpp
    cmake ./CMakeLists.txt && make -j$(nproc) >compile_log.txt 2>&1
    mv ./capi $output/capi
    if [ $? -eq 0 ]; then
        flag=0
    fi
    mv ./compile_log.txt $output/compile_log.txt
else
    flag=0
fi

if [ $flag -eq 1 ]; then
    curl $URL -X PUT -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d '{"compile_status":"compiled"}' > $bind/curl_log.txt
else
    curl $URL -X PUT -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d '{"compile_status":"failed"}' > $bind/curl_log.txt
fi
