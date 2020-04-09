cpp_file=''

#for file in *.cpp;
#do
#    cpp_file="$file $cpp_file"
#done
#for file in utility/*.cpp;
#do
#    cpp_file="$file $cpp_file"
#done
for file in unitests/*.cpp;
do
    cpp_file="$file $cpp_file"
done

echo "src files:" $cpp_file

/opt/compiler/gcc-8.2/bin/g++ -o tinynn_main $cpp_file -std=c++2a -lpthread -I. -I thirdparty/gflags/include/ -I thirdparty/gtest/include/ -L thirdparty/gflags/lib/ -L thirdparty/gtest/lib/ -lgflags -lgtest -lgtest_main
