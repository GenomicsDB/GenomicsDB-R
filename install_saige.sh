#!/bin/sh

OS=`uname -s`
if [[ $OS != "Linux" ]]; then
    echo "$OS operation system is not supported by SAIGE. Exiting installation"
    exit 1
fi

INSTALL_DIR=`pwd`
if [ $# -eq 1 ];then
    INSTALL_DIR=$1
fi

if [ -d $INSTALL_DIR/SAIGE ]; then
    echo "$INSTALL_DIR/SAIGE already exists. Exiting installation."
    exit 1
else
  echo"Installing SAIGE in $INSTALL_DIR directory"
fi

pushd $INSTALL_DIR

src_branch=master
repo_src_url=https://github.com/weizhouUMICH/SAIGE
git clone --depth 1 -b $src_branch $repo_src_url

# Install SAIGE dependencies
R -e 'install.packages("R.utils", "Rcpp", "RcppParallel", "RcppArmadillo", "data.table", "RcppEigen", "Matrix", "methods", "BH", "optparse", "SPAtest", "MetaSKAT", "SKAT", "roxygen2", "rversions","devtools", repos="http://cran.us.r-project.org")'

pushd SAIGE

R CMD build .
R CMD INSTALL SAIGE_0.36.3.1.tar.gz

popd
popd
