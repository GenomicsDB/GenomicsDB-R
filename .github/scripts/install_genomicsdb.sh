#!/bin/bash

GENOMICSDB_DIR=$HOME/GenomicsDB
GENOMICSDB_BRANCH=develop
GENOMICSDB_BUILD_DIR=$GENOMICSDB_DIR/build
GENOMICSDB_INSTALL_DIR=$GENOMICSDB_DIR/release

CMAKE=cmake
BUILD_FOR_PYTHON=true

install_genomicsdb() {
  echo "Starting install of GenomicsDB"
  if  [[ $(uname) == "Darwin" ]]; then export MACOSX_DEPLOYMENT_TARGET=10.13; fi &&
	git clone https://github.com/GenomicsDB/GenomicsDB --recursive -b $GENOMICSDB_BRANCH $GENOMICSDB_DIR &&
      INSTALL_PREFIX=$HOME/genomicsdb_prereqs $GENOMICSDB_DIR/scripts/prereqs/install_prereqs.sh &&
      if [[ $(uname) == "Darwin" ]]; then brew install open-mpi; fi &&
      source $HOME/genomicsdb_prereqs.sh &&
			mkdir $GENOMICSDB_BUILD_DIR &&
			pushd $GENOMICSDB_BUILD_DIR &&
      if [[ -n "$IPPROOT" ]]; then
         echo "  $CMAKE .. -DCMAKE_INSTALL_PREFIX=$GENOMICSDB_INSTALL_DIR -DIPPROOT=$IPPROOT -DBUILD_DISTRIBUTABLE_LIBRARY=1 -DBUILD_JAVA=0 -DUSE_HDFS=0 -DBUILD_FOR_PYTHON=$BUILD_FOR_PYTHON" &&
         $CMAKE .. -DCMAKE_INSTALL_PREFIX=$GENOMICSDB_INSTALL_DIR -DIPPROOT=$IPPROOT -DBUILD_DISTRIBUTABLE_LIBRARY=1 -DBUILD_JAVA=0 -DUSE_HDFS=0 -DBUILD_FOR_PYTHON=$BUILD_FOR_PYTHON && make
      else
         echo "  $CMAKE .. -DCMAKE_INSTALL_PREFIX=$GENOMICSDB_INSTALL_DIR -DBUILD_DISTRIBUTABLE_LIBRARY=1 -DBUILD_JAVA=0 -DUSE_HDFS=0 -DBUILD_FOR_PYTHON=$BUILD_FOR_PYTHON" &&
         $CMAKE .. -DCMAKE_INSTALL_PREFIX=$GENOMICSDB_INSTALL_DIR -DBUILD_DISTRIBUTABLE_LIBRARY=1 -DBUILD_JAVA=0 -DUSE_HDFS=0 -DBUILD_FOR_PYTHON=$BUILD_FOR_PYTHON && make
      fi
			make -j 4 &&
			make install &&
			test -f $GENOMICSDB_INSTALL_DIR/include/genomicsdb.h &&
			test -f $GENOMICSDB_INSTALL_DIR/lib/libtiledbgenomicsdb.so &&
			popd
}

install_genomicsdb &&
export GENOMICSDB_HOME=$GENOMICSDB_INSTALL_DIR
