library(Rcpp)
library(genomicsdb)

tmp_dir <- tempdir()
resources_file <- "tests/inputs/sanity.test.tgz"
untar(resources_file, exdir=tmp_dir)
setwd(tmp_dir)

genomicsdb::version()

gdb <- genomicsdb::connect(workspace = "ws", vid_mapping_file = "vid.json", callset_mapping_file = "callset_t0_1_2.json", reference_genome="chr1_10MB.fasta.gz", ("DP"), 40)

# Test Query Variants - Returns List
genomicsdb::query_variants(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,1000000000)), row_ranges=list(c(0,3)))

# Test Query VariantCalls - Return List of Query Intervals by Row
genomicsdb::query_variant_calls(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,1000000000)), row_ranges=list(c(0,3)))
genomicsdb::query_variant_calls(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,150000), c(15001, 1000000000)), row_ranges=list(c(0,3)))

genomicsdb::disconnect(genomicsdb=gdb)

# Test Generate VCF and connect_with_query_json
gdb <- genomicsdb::connect_with_query_json(query_configuration_json_file="query.json", loader_configuration_json_file="loader.json")
output <- "no_spark.vcf.gz"
genomicsdb::generate_vcf(genomicsdb=gdb, output=output, output_format="z", overwrite=FALSE)
genomicsdb::disconnect(genomicsdb=gdb)
stopifnot(file.exists(output) && file.exists(paste(output,".tbi",sep="")))

unlink(tmp_dir)
