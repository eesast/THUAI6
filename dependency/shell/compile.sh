#! /bin/bash 
# WORKDIR /usr/local/PlayerCode/CAPI/cpp
i=1
flag=1
bind=/usr/local/mnt
while (( $i <= 4 ))
do
    cp -f $bind/player$i.cpp ./API/src
    mv ./API/src/player$i.cpp ./API/src/AI.cpp
    cmake ./CMakeLists.txt && make >compile_log$i.txt 2>&1
    mv ./capi $bind/capi$i # executable file
    if [ $? -ne 0 ]; then
        flag=0
    fi
    mv ./compile_log$i.txt $bind/compile_log$i.txt
    let "i++"
done
# HTML request to update status.
if [ $flag -eq 1 ]; then
    curl $URL -X PUT -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d '{"compile_status":"compiled"}' > $bind/curl_log.txt
else
    curl $URL -X PUT -H "Content-Type: application/json" -H "Authorization: Bearer $TOKEN" -d '{"compile_status":"failed"}' > $bind/curl_log.txt
fi