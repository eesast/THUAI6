clang-format --version
if [ $? -eq 0 ]
then
    for i in {1..10}
    do
    find . -iname "*.cs"  \
    -or -iname "*.c"   \
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
fi
clang-format-15 --version
if [ $? -eq 0 ]
then
    for i in {1..10}
    do
    find . -iname "*.cs"  \
    -or -iname "*.c"   \
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
    | xargs clang-format-15 -i
    done
fi
clang-format-14 --version
if [ $? -eq 0 ]
then
    for i in {1..10}
    do
    find . -iname "*.cs"  \
    -or -iname "*.c"   \
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
    | xargs clang-format-14 -i
    done
fi