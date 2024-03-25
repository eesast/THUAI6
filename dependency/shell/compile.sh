#! /bin/bash 
# WORKDIR /usr/local/PlayerCode/CAPI/cpp
workdir=/usr/local/PlayerCode/CAPI/cpp
bind=/usr/local/code
output=/usr/local/code

flag=1

cd $bind
file_count=$(ls -l *.cpp | wc -l);
if [ $file_count -eq 1 ]
then
    filename=$(ls *.cpp)
    base_name=$(basename "$filename" .cpp)
    cd $workdir
    cp -f $bind/$filename $workdir/API/src/AI.cpp
    cmake ./CMakeLists.txt && make -j$(nproc) >$base_name.txt 2>&1
    mv ./capi $output/$base_name
    if [ $? -ne 0 ]; then
        flag=0
    fi
    mv ./$base_name.txt $output/$base_name.txt
else
    flag=0
fi

if [ $flag -eq 1 ]; then
    curl $URL -X POST -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d '{"compile_status":"Success"}' > $bind/curl_log.txt
else
    curl $URL -X POST -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d '{"compile_status":"Failed"}' > $bind/curl_log.txt
fi
