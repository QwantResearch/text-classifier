echo "**********************************"
echo "******* Compilation **************"
echo "**********************************"


rm -f fasttext_pybind.so ../fasttext_pybind.so
# c++ -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` translate.cc -o qtranslate`python3-config --extension-suffix` -I/usr/local/include/eigen3 -I/usr/local/include/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/ g++ -fPIC -g -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` pybind/fasttext_pybind.cc -o fasttext_pybind.so -I/usr/local/include/eigen3 -I/usr/local/include/ -L/usr/local/lib/ -I/usr/local/include/fasttext/ -I/usr/include/python3.4m/ -I/usr/include/python3.5m/ -lfasttext 
echo "Releasing..."
rsync -a --progress fasttext_pybind* ../ 
