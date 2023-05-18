# GenomicsDB-R
Experimental R bindings to the native [GenomicsDB](https://github.com/GenomicsDB/GenomicsDB) library. Only queries are supported for now. For ingestion, use GenomicsDB native command line tool [vcf2genomicsdb](https://genomicsdb.readthedocs.io/en/latest/cli-tools.html#cli-tools) or [gatk GenomicsDBImport](https://gatk.broadinstitute.org/hc/en-us/articles/13832686645787-GenomicsDBImport)

## Installation

### Prerequisites
See [GenomicsDB build/install](https://genomicsdb.readthedocs.io/en/latest/building-installing.html) for a docker or a scripted install of GenomicsDB for MacOS and Linux based systems. Use the installed GemomicsDB path as input to GenomicsDB-R configuration. See `<path_to_genomicsdb> below.

### Installation from [Github](https://github.com/GenomicsDB/GenomicsDB-R) using [remotes](https://cran.r-project.org/package=remotes)
```
From R/RStudio
library(remotes)
remotes::install_github("GenomicsDB/GenomicsDB-R", ref="<github_branch>", configure.args="--with-genomicsdb=<path_to_genomicsdb>")
```
