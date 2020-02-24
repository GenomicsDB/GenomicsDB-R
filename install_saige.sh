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
R -e 'install.packages("R.utils", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("Rcpp", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("RcppParallel", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("RcppArmadillo", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("data.table", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("RcppEigen", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("Matrix", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("methods", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("BH", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("optparse", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("SPAtest", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("MetaSKAT", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("SKAT", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("roxygen2", repos = "http://cran.us.r-project.org")'
R -e 'install.packages("rversions", repos = "http://cran.us.r-project.org")'

pushd SAIGE

R CMD build .
R CMD INSTALL SAIGE_0.36.3.1.tar.gz

popd
popd
