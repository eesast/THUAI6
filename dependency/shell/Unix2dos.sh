#!/bin/bash
function lsdir()
{
for i in `ls`;
do
        if [ -d $i ]
        then
                pushd ./$i
                lsdir
                popd
        else
                sed -i 's/$/\r/g' $i
        fi
done
}
cd ../../
lsdir
