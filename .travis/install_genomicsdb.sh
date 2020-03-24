#!/bin/bash

GENOMICSDB_DIR=$HOME/GenomicsDB
GENOMICSDB_BRANCH=develop
GENOMICSDB_BUILD_DIR=$GENOMICSDB_DIR/build
GENOMICSDB_INSTALL_DIR=$GENOMICSDB_DIR/release
PATH=$HOME/bin:$PATH

install_prerequisites() {
  echo "Installing Prerequisites"
	sudo apt-get -y install lcov mpich zlib1g-dev libssl-dev rsync cmake uuid-dev &&
			sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y &&
			sudo add-apt-repository -y ppa:openjdk-r/ppa &&
			sudo apt-get update -q &&
			sudo apt-get install gcc-4.9 -y &&
			sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 &&
			sudo apt-get install g++-4.9 -y &&
			sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 60 &&
			sudo apt-get -y install openjdk-8-jdk icedtea-plugin &&
			sudo apt-get -y install zip unzip &&
			mkdir protobuf_tmp && pushd protobuf_tmp && wget -nv https://github.com/GenomicsDB/GenomicsDB/releases/download/v1.0.0/protobuf-3.0.2-trusty.tar.gz -O protobuf-3.0.2-trusty.tar.gz &&
			tar xzf protobuf-3.0.2-trusty.tar.gz && sudo rsync -a protobuf-3.0.2-trusty/ /usr/ && popd && rm -fr protobuf_tmp &&
			jdk_switcher use openjdk8
}

install_genomicsdb() {
  echo "Starting install of GenomicsDB"
	git clone https://github.com/GenomicsDB/GenomicsDB -b $GENOMICSDB_BRANCH $GENOMICSDB_DIR &&
			pushd $GENOMICSDB_DIR && git submodule update --recursive --init && popd &&
			mkdir $GENOMICSDB_BUILD_DIR &&
			pushd $GENOMICSDB_BUILD_DIR &&
			cmake $GENOMICSDB_DIR -DCMAKE_INSTALL_PREFIX=$GENOMICSDB_INSTALL_DIR -DENABLE_CURL=0 &&
			make -j 4 &&
			make install &&
			test -f $GENOMICSDB_INSTALL_DIR/include/genomicsdb.h &&
			test -f $GENOMICSDB_INSTALL_DIR/lib/libtiledbgenomicsdb.so &&
			popd
}

install_prerequisites &&
install_genomicsdb &&
export GENOMICSDB_HOME=$GENOMICSDB_INSTALL_DIR
