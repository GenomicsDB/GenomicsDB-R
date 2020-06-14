#!/bin/bash

GENOMICSDB_DIR=$HOME/GenomicsDB
GENOMICSDB_BRANCH=develop
GENOMICSDB_BUILD_DIR=$GENOMICSDB_DIR/build
GENOMICSDB_INSTALL_DIR=$GENOMICSDB_DIR/release
PATH=$HOME/bin:$PATH

install_protobuf() {
  echo "Installing Protobuf"
  pushd /tmp
  git clone -b 3.0.x --single-branch https://github.com/google/protobuf.git &&
  pushd protobuf &&
  ./autogen.sh &&
  ./configure --prefix=/usr --with-pic &&
  make -j4 && sudo make install &&
  echo "Installing Protobuf DONE"
  popd
  rm -fr /tmp/protobuf*
  popd
}

install_prerequisites() {
  echo "Installing Prerequisites"
	sudo apt-get -y install zlib1g-dev libssl-dev cmake uuid-dev &&
		sudo apt-get update -q &&
		install_protobuf
}

install_genomicsdb() {
  echo "Starting install of GenomicsDB"
	git clone https://github.com/GenomicsDB/GenomicsDB -b $GENOMICSDB_BRANCH $GENOMICSDB_DIR &&
			pushd $GENOMICSDB_DIR && git submodule update --recursive --init && popd &&
			mkdir $GENOMICSDB_BUILD_DIR &&
			pushd $GENOMICSDB_BUILD_DIR &&
			cmake $GENOMICSDB_DIR -DCMAKE_INSTALL_PREFIX=$GENOMICSDB_INSTALL_DIR -DENABLE_LIBCURL=0 -DDISABLE_MPI=1 &&
			make -j 4 &&
			make install &&
			test -f $GENOMICSDB_INSTALL_DIR/include/genomicsdb.h &&
			test -f $GENOMICSDB_INSTALL_DIR/lib/libtiledbgenomicsdb.so &&
			popd
}

install_prerequisites &&
install_genomicsdb &&
export GENOMICSDB_HOME=$GENOMICSDB_INSTALL_DIR
