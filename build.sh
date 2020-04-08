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

g++ -o dup $cpp_file -std=c++2a -lpthread -I. 
