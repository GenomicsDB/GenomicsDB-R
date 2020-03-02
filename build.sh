#!/bin/bash

./cleanup

GENOMICSDB_HOME=$HOME

# R -e 'library(Rcpp); compileAttributes(".")'
R CMD build .
R CMD INSTALL --preclean --configure-args="--with-genomicsdb=${GENOMICSDB_HOME}" genomicsdb_0.0.2.tar.gz
R CMD check --no-manual --install-args="--configure-args='--with-genomicsdb=${GENOMICSDB_HOME}'" genomicsdb_0.0.2.tar.gz
R -e 'library(devtools); test()'
