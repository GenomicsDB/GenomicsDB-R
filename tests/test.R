library(Rcpp)
library(genomicsdb)

tmp_dir <- tempdir()
resources_file <- "tests/inputs/sanity.test.tgz"
untar(resources_file, exdir=tmp_dir)
setwd(tmp_dir)

genomicsdb::version()

gdb <- genomicsdb::setup(workspace = file.path(getwd(), "ws"), vid_mapping_file = "vid.json", callset_mapping_file = "callset_t0_1_2.json", reference_genome="chr1_10MB.fasta.gz", ("DP"))

# Test Query Variants - Returns List
genomicsdb::query_variants(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,1000000000)), row_ranges=list(c(0,3)))

# Test Query VariantCalls - Return List of Query Intervals by Row
genomicsdb::query_variant_calls(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,1000000000)), row_ranges=list(c(0,3)))
genomicsdb::query_variant_calls(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,150000), c(15001, 1000000000)), row_ranges=list(c(0,3)))

unlink(tmp_dir)

