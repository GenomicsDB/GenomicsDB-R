#!/bin/bash

R -e 'library(Rcpp); compileAttributes(".")'
R CMD build .
R CMD INSTALL --preclean --configure-args="--with-genomicsdb=${HOME}" genomicsdb_0.0.1.tar.gz
