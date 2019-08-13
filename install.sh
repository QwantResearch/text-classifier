#!/usr/bin/env bash

export PREFIX=/usr/local/

echo "Prefix set to $PREFIX"

export CMAKE_PREFIX_PATH=$PREFIX

git submodule update --init --recursive

echo "Installing dependencies"

pushd vendor/qnlp-toolkit
	rm -rf build
	git pull  --recurse-submodules 
	bash install.sh $PREFIX
popd
 
for dep in pistache json
do
pushd vendor/$dep
	#git submodule update --init
	rm -rf build
	mkdir -p build
	pushd build
		cmake .. -DCMAKE_INSTALL_PREFIX="${PREFIX}" -DCMAKE_BUILD_TYPE=Release
		make -j 4 && make install
	popd
popd
done

pushd vendor/grpc
	git submodule update --init
	rm -rf build
	mkdir -p build
	pushd build
		cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DgRPC_PROTOBUF_PROVIDER=package -DgRPC_ZLIB_PROVIDER=package -DgRPC_CARES_PROVIDER=package -DCMAKE_BUILD_TYPE=Release --DCMAKE_INSTALL_PREFIX="/usr/local/" ..
		# See https://github.com/grpc/grpc/issues/13841
		# Appears to be a problem with multi-threading, if make -j 4 doesn't work, use make -j 1
		make -j 1 && make install
	popd
	make && make install
popd

echo "Installing text-classifier"
mkdir -p $PREFIX
rm -rf build
mkdir -p build
pushd build
	cmake .. -DCMAKE_INSTALL_PREFIX="${PREFIX}" -DCMAKE_BUILD_TYPE=Release 
	make -j 4 && make install
popd
