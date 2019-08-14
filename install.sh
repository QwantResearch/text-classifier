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

	# Based on https://github.com/grpc/grpc/blob/master/test/distrib/cpp/run_distrib_test_cmake.sh

	# Install c-ares
	pushd third_party/cares/cares
		git fetch origin
		git checkout cares-1_15_0
		mkdir -p cmake/build
		pushd cmake/build
			cmake -DCMAKE_BUILD_TYPE=Release ../..
			make -j4 install
		popd
	popd
	# rm -rf third_party/cares/cares  # wipe out to prevent influencing the grpc build

	# Install zlib
	pushd third_party/zlib
		mkdir -p cmake/build
		pushd cmake/build
			cmake -DCMAKE_BUILD_TYPE=Release ../..
			make -j4 install
		popd
	popd
	# rm -rf third_party/zlib  # wipe out to prevent influencing the grpc build

	# Install protobuf
	pushd third_party/protobuf
		mkdir -p cmake/build
		pushd cmake/build
			cmake -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ..
			make -j4 install
		popd
	popd
	# rm -rf third_party/protobuf  # wipe out to prevent influencing the grpc build

	# Install gRPC
	#mkdir -p cmake/build
	#pushd cmake/build
	# cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DgRPC_PROTOBUF_PROVIDER=package -DgRPC_ZLIB_PROVIDER=package -DgRPC_CARES_PROVIDER=package -DgRPC_SSL_PROVIDER=package -DCMAKE_BUILD_TYPE=Release ../..
	# make -j4 install
	#pushd ../..

	rm -rf cmake/build
	mkdir -p cmake/build
	pushd cmake/build
		cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DgRPC_PROTOBUF_PROVIDER=package -DgRPC_ZLIB_PROVIDER=package -DgRPC_CARES_PROVIDER=package -DgRPC_SSL_PROVIDER=package -DCMAKE_BUILD_TYPE=Release --DCMAKE_INSTALL_PREFIX="/usr/local/" ../..
		# See https://github.com/grpc/grpc/issues/13841
		# Appears to be a problem with multi-threading, if make -j 4 doesn't work, use make -j 1
			make -j 4 && make install
		popd
popd

echo "Installing text-classifier"
mkdir -p $PREFIX
rm -rf build
mkdir -p build
pushd build
	cmake .. -DCMAKE_INSTALL_PREFIX="${PREFIX}" -DCMAKE_BUILD_TYPE=Release 
	make -j 4 && make install
popd
