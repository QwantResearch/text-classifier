rm -rf build
mkdir -p build 
pushd build 
cmake .. && make -j 40
rm -rfv ../../qtokenizer.so
rsync -a --progress libqtokenizer.so ../../qtokenizer.so 
popd

