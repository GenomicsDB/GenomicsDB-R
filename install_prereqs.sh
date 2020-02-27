#!/bin/bash

if [ `uname -s` == "Linux" ]; then
  sudo yum install -y libcurl-devel
  sudo yum install -y R
fi

# Allows for R_LIB_PATH to be configured for packages, etc. to be installed in
R_LIBS_PATH="$HOME/R-libs"
if [ ! -d $R_LIBS_PATH ]; then
  mkdir $R_LIBS_PATH
fi
echo "R_LIBS=$R_LIBS_PATH" > .Renviron

R -e 'install.packages("devtools", dependencies=TRUE, repos="https://cloud.r-project.org")'

