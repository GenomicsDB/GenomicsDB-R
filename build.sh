#!/bin/bash

./cleanup

if [[ "test$GENOMICSDB_HOME" == "test" ]]; then
   GENOMICSDB_HOME=$HOME
fi

echo "Using GENOMICSDB_HOME=${GENOMICSDB_HOME}"

R -e 'library(Rcpp); compileAttributes(".")'
R CMD build . &&
R CMD INSTALL --preclean --configure-args="--with-genomicsdb=${GENOMICSDB_HOME}" genomicsdb_0.0.3.tar.gz &&
R CMD check --no-manual --install-args="--configure-args='--with-genomicsdb=${GENOMICSDB_HOME}'" genomicsdb_0.0.3.tar.gz &&
R -e 'library(devtools); test()'
