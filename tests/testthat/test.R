library(Rcpp)
library(genomicsdb)

test_that("version is valid", {
    version <- genomicsdb::version()
    expect_vector(version, ptype = character())
})

print(getwd())
tmp_dir <- tempdir()
resources_file <- "inputs/sanity.test.tgz"
untar(resources_file, exdir=tmp_dir)
setwd(tmp_dir)


test_that("test that genomicsdb connects to an existing workspace for queries", {
    gdb <- genomicsdb::connect(workspace = "ws", vid_mapping_file = "vid.json", callset_mapping_file = "callset_t0_1_2.json", c("DP"), 40)
    expect_type(gdb, "externalptr")

    # Test Query Variants - Returns List
    variants <- genomicsdb::query_variants(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,1000000000)), row_ranges=list(c(0,3)))
    expect_length(variants, 1)

    # Test Query VariantCalls - Return List of Query Intervals by Row
    variantcalls <- genomicsdb::query_variant_calls(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,1000000000)), row_ranges=list(c(0,3)))
    expect_length(variantcalls, 9) # Number of columns
    expect_true(nrow(variantcalls) == 5) # Number of rows
          
    variantcalls <- genomicsdb::query_variant_calls_by_interval(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,1000000000)), row_ranges=list(c(0,3)))
    expect_length(variantcalls, 1)

    variantcalls1 <- genomicsdb::query_variant_calls(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,150000), c(15001, 1000000000)), row_ranges=list(c(0,3)))
    expect_length(variantcalls1, 9) # Number of columns
    expect_true(nrow(variantcalls1) == 8) # Number of rows
    variantcalls1 <- genomicsdb::query_variant_calls_by_interval(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,150000), c(15001, 1000000000)), row_ranges=list(c(0,3)))
    expect_length(variantcalls1, 1)

    # Test generate_vcf
    output <- "generated_no_spark.vcf.gz"
    genomicsdb::generate_vcf(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,150000), c(15001, 1000000000)), row_ranges=list(c(0,3)),
                             reference_genome="chr1_10MB.fasta.gz", vcfheader_template <- "template_vcf_header.vcf", output=output, output_format="z", overwrite=FALSE)
    expect_true(file.exists(output))
    expect_true(file.exists(paste(output,".tbi",sep="")))
    
    genomicsdb::disconnect(genomicsdb=gdb)
})

test_that("test that genomicsdb can output genotypes as strings", {
    gdb <- genomicsdb::connect(workspace = "ws", vid_mapping_file = "vid.json", callset_mapping_file = "callset_t0_1_2.json", c("DP", "GT"), 40)
    expect_type(gdb, "externalptr")

    # Test Query Variants - Returns List of Query Intervals by Row
    variantcalls <- genomicsdb::query_variant_calls(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,150000), c(15001, 1000000000)), row_ranges=list(c(0,3)))
    expect_length(variantcalls, 10) # Number of columns
    expect_true(nrow(variantcalls) == 8) # Number of rows
    print(variantcalls)
    
    variantcalls <- genomicsdb::query_variant_calls_by_interval(genomicsdb=gdb, array="t0_1_2", column_ranges=list(c(0,150000), c(15001, 1000000000)), row_ranges=list(c(0,3)))
    expect_length(variantcalls, 1)
    
    genomicsdb::disconnect(genomicsdb=gdb)
})


test_that("test that genomicsdb connect to an existing workspace through query json file for queries", {
    gdb <- genomicsdb::connect_with_query_json(query_configuration_json_file="query.json", query_configuration_type=0, loader_configuration_json_file="loader.json")
    expect_type(gdb, "externalptr")

    output <- "no_spark.vcf.gz"
    genomicsdb::generate_vcf_json(genomicsdb=gdb, output=output, output_format="z", overwrite=FALSE)
    expect_true(file.exists(output))
    expect_true(file.exists(paste(output,".tbi",sep="")))
    
    genomicsdb::disconnect(genomicsdb=gdb)
})

unlink(tmp_dir)
