if (nchar(Sys.getenv("SPARK_HOME")) < 1) {
  Sys.setenv(SPARK_HOME = file.path(Sys.getenv("HOME"), "spark"))
}
spark_home <- Sys.getenv("SPARK_HOME")
print(spark_home)
stopifnot(dir.exists(spark_home))

library(Rcpp)
tmp_dir <- tempdir()
resources_file <- "tests/inputs/sanity.test.tgz"
untar(resources_file, exdir=tmp_dir)
setwd(tmp_dir)

# Test with SparkR
library(SparkR, lib.loc = c(file.path(Sys.getenv("SPARK_HOME"), "R", "lib")))
sparkR.session(master = "local[*]", sparkConfig = list(spark.driver.memory = "2g"))

generate_vcf <- spark.lapply(list(list("query.json", "loader.json", "output")), function(inputs) {
  library(genomicsdb)
  gdb <- genomicsdb::connect_with_query_json(query_configuration_json_file=inputs[[1]], loader_configuration_json_file=inputs[[2]])
  output <- as.character(paste(inputs[[3]],".vcf.gz",sep=""))
  genomicsdb::generate_vcf(genomicsdb=gdb, output=output, output_format="z", overwrite=FALSE)
  genomicsdb::disconnect(genomicsdb=gdb)
  return(file.exists(output) && file.exists(paste(output,".tbi",sep="")))
})

print(generate_vcf)

unlink(tmp_dir)
