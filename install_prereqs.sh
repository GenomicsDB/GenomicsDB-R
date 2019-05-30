#!/bin/bash

R_LIBS_PATH="$HOME/R-libs"
if [ ! -d $R_LIBS_PATH ]; then
  mkdir $R_LIBS_PATH
fi

sudo yum install -y libcurl-devel
sudo yum install -y R
echo "R_LIBS=$R_LIBS_PATH" > .Renviron
R -e 'install.packages("devtools", dependencies=TRUE, repos="http://cran.us.r-project.org")'

