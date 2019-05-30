library(Rcpp)
library(genomicsdb)

tmp_dir <- tempdir()
resources_file <- "tests/inputs/sanity.test.tgz"
untar(resources_file, exdir=tmp_dir)
setwd(tmp_dir)

genomicsdb::version()

gdb <- genomicsdb::setup(workspace = file.path(getwd(), "ws"), vid_mapping_file = "vid.json", callset_mapping_file = "callset_t0_1_2.json", reference_genome="chr1_10MB.fasta.gz", ("DP"))
genomicsdb::query_variants(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,1000000000)), row_ranges=list(c(0,3)))

unlink(tmp_dir)

