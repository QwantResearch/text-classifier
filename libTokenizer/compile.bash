echo "**********************************"
echo "******* Compilation **************"
echo "**********************************"


rm -f qtokenizer.so ../qtokenizer.so
# c++ -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` translate.cc -o qtranslate`python3-config --extension-suffix` -I/usr/local/include/eigen3 -I/usr/local/include/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/ g++ -fPIC -g -O3 -Wall -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` src/qtokenizer.cpp src/tokenizer.cpp -o qtokenizer.so -I/usr/local/include/eigen3 -I/usr/local/include/ -L/usr/local/lib/ -I./include -I/usr/include/python3.4m/ -lonmt
echo "Releasing..."
rsync -a --progress qtokenizer* ../ 
