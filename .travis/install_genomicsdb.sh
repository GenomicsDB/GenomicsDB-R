#!/bin/bash

GENOMICSDB_DIR=$TRAVIS_BUILD_DIR/GenomicsDB
GENOMICSDB_BUILD_DIR=$GENOMICSDB_DIR/build
GENOMICSDB_INSTALL_DIR=$TRAVIS_BUILD_DIR
PATH=$TRAVIS_BUILD_DIR/bin:$PATH

install_prerequisites() {
	sudo apt-get -y install lcov mpich zlib1g-dev libssl-dev rsync cmake uuid-dev libcurl4-openssl-dev &&
			sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y &&
			sudo add-apt-repository -y ppa:openjdk-r/ppa &&
			sudo apt-get update -q &&
			sudo apt-get install gcc-4.9 -y &&
			sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 &&
			sudo apt-get install g++-4.9 -y &&
			sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 60 &&
			sudo apt-get -y install openjdk-8-jdk icedtea-plugin &&
			sudo apt-get -y install zip unzip &&
			wget -nv https://github.com/GenomicsDB/GenomicsDB/releases/download/v1.0.0/protobuf-3.0.2-trusty.tar.gz -O protobuf-3.0.2-trusty.tar.gz &&
			tar xzf protobuf-3.0.2-trusty.tar.gz && sudo rsync -a protobuf-3.0.2-trusty/ /usr/ &&
			jdk_switcher use openjdk8
}

install_genomicsdb() {
	git clone https://github.com/GenomicsDB/GenomicsDB -b nalini_api $GENOMICSDB_DIR &&
			pushd $GENOMICSDB_DIR && git submodule update --recursive --init && popd &&
			mkdir $GENOMICSDB_BUILD_DIR &&
			pushd $GENOMICSDB_BUILD_DIR &&
			cmake $GENOMICSDB_DIR -DCMAKE_INSTALL_PREFIX=$GENOMICSDB_INSTALL_DIR &&
			make -j 4 &&
			make install &&
			test -f $GENOMICSDB_INSTALL_DIR/include/genomicsdb.h &&
			test -f $GENOMICSDB_INSTALL_DIR/lib/libtiledbgenomicsdb.so &&
			popd
}

install_prerequisites &&
install_genomicsdb
