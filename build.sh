#!/bin/bash

./configure

R -e 'library(Rcpp); compileAttributes(".")'
R CMD build .
R CMD INSTALL --preclean --configure-args="--with-genomicsdb=${HOME}" genomicsdb_0.0.2.tar.gz
