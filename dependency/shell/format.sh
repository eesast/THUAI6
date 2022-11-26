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

cd logic && dotnet format && cd ..
cd installer && dotnet format && cd ..
cd launcher && dotnet format && cd ..
cd playback && dotnet format && cd ..

echo "Done!"