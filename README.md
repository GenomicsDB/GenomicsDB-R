# GenomicsDB-R
Experimental R bindings to the native [GenomicsDB](https://github.com/GenomicsDB/GenomicsDB) library. Only queries are supported for now. For ingestion, use the command line tool - `[vcf2genomicsdb](https://github.com/GenomicsDB/GenomicsDB/wiki/Importing-VCF-data-into-GenomicsDB)` or [gatk GenomicsDBImport](https://gatk.broadinstitute.org/hc/en-us/articles/360047216891-GenomicsDBImport)

## Installation

### Prerequisites
See [GenomicsDB-install](https://github.com/GenomicsDB/GenomicsDB-Install) for a docker or a scripted install of GenomicsDB for Linux based systems. For example, on an `ubuntu` system -
```
git clone https://github.com/GenomicsDB/GenomicsDB-Install.git
cd GenomicsDB-Install
docker build --build-arg os=ubuntu --build-arg branch=master --build-arg distributable_jar=true --build-arg install_dir=/tmp/genomicsdb/install -t genomicsdb:build .
docker create -it --name genomicsdb genomicsdb:build bash
docker cp genomicsdb:/tmp/genomicsdb/install <path_to_genomicsdb>
docker rm -fv genomicsdb
```

### Installation from [Github](https://github.com/GenomicsDB/GenomicsDB-R) using [remotes](https://cran.r-project.org/package=remotes)
```
From R/RStudio
library(remotes)
remotes::install_github("GenomicsDB/GenomicsDB-R", ref="<github_branch>", configure.args="--with-genomicsdb=<path_to_genomicsdb>")
```
