SHELL_FOLDER=$(dirname $(readlink -f "$0"))
cd $SHELL_FOLDER
cd ../..

for i in {1..3}
do
find . -iname "*.c"   \
   -or -iname "*.h"   \
   -or -iname "*.C"   \
   -or -iname "*.H"   \
   -or -iname "*.cpp" \
   -or -iname "*.hpp" \
   -or -iname "*.cc"  \
   -or -iname "*.hh"  \
   -or -iname "*.c++" \
   -or -iname "*.h++" \
   -or -iname "*.cxx" \
   -or -iname "*.hxx" \
   -or -iname "*.i"   \
   -or -iname "*.ixx" \
   -or -iname "*.ipp" \
   -or -iname "*.i++" \
   | xargs clang-format -i
done

pushd logic && dotnet format && popd
pushd installer && dotnet format && popd
pushd launcher && dotnet format && popd
pushd playback && dotnet format && popd

echo "Done!"
