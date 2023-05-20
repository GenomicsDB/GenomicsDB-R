#!/bin/bash

./cleanup

if [[ "test$GENOMICSDB_HOME" == "test" ]]; then
   GENOMICSDB_HOME=$HOME
fi

echo "Using GENOMICSDB_HOME=${GENOMICSDB_HOME}"

GENOMICSDS_R_VERSION=$(grep Version DESCRIPTION | awk '{print $2}')
echo "GENOMICS_DB_R_VERSION=$GENOMICSDS_R_VERSION"

R -e 'library(Rcpp); compileAttributes(".")'
R CMD build . &&
R CMD INSTALL --preclean --configure-args="--with-genomicsdb=${GENOMICSDB_HOME}" genomicsdb_$GENOMICSDS_R_VERSION.tar.gz &&
R CMD check --no-manual --install-args="--configure-args='--with-genomicsdb=${GENOMICSDB_HOME}'" genomicsdb_$GENOMICSDS_R_VERSION.tar.gz &&
R -e 'library(devtools); test()'
