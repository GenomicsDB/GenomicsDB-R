#!/bin/bash

rm -f  src/Makevars src/*.o src/*.so config.log config.status
rm -fr genomicsdb.Rcheck

./configure --with-genomicsdb=${HOME}

# R -e 'library(Rcpp); compileAttributes(".")'
R CMD build .
R CMD install --preclean --configure-args="--with-genomicsdb=${HOME}" genomicsdb_0.0.2.tar.gz
R CMD check --no-manual genomicsdb_0.0.2.tar.gz
R -e 'library(devtools); test()'
