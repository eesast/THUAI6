#! /bin/bash
workdir=/d/伤风/软件部/MyTHUAI6/THUAI6/logic
targetdir=/d/伤风/软件部/THUAI6-毕业吧少女

mkdir -p ${targetdir}

pushd ${targetdir}
mkdir -p win/win64
mkdir -p linux/linux64/
mkdir -p win/win64/Debug/
mkdir -p linux/linux64/Debug/
popd

pushd ${workdir}

pushd Server/bin/Release/net6.0/linux-x64/publish
rm *.pdb
cp -rf * ${targetdir}/linux/linux64/
popd

pushd Server/bin/Debug/net6.0/linux-x64/publish
cp -rf * ${targetdir}/linux/linux64/Debug/
popd

pushd Server/bin/Debug/net6.0/win-x64/publish
cp -rf * ${targetdir}/win/win64/Debug/
popd

pushd Server/bin/Release/net6.0/win-x64/publish
rm *.pdb
cp -rf * ${targetdir}/win/win64/
popd

pushd Client/bin/Release/net6.0-windows/win-x64/publish
rm *.pdb
cp -rf * ${targetdir}/win/win64/

popd
