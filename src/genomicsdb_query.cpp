/**
 * @file genomicsdb_query.cpp
 *
 * @section LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Omics Data Automation, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * Rcpp code to interface with the genomicsdb library
 *
 **/

#include <RcppCommon.h>
#include "genomicsdb_types.h"

// Custom "wrap" and "as" templates are required to be between RcppCommon.h and Rcpp.h
namespace Rcpp
{
  template<typename T>
  SEXP wrap(const std::pair<T, T>& pair);
  template<>
  SEXP wrap(const std::pair<uint64_t, uint64_t>& pair);

  template<>
  SEXP wrap(const genomic_interval_t& interval);

  template<>
  SEXP wrap(const std::vector<genomic_field_t>& genomic_fields);

}

#include <Rcpp.h>
#include <exception>
#include <iostream>

template<typename T>
SEXP Rcpp::wrap(const std::pair<T, T>& pair) {
  std::vector<SEXP> pair_vector = {Rcpp::wrap(pair.first), Rcpp::wrap(pair.second)};
  return Rcpp::wrap(pair_vector);
}

template<>
SEXP Rcpp::wrap(const std::pair<uint64_t, uint64_t>& pair) {
  return Rcpp::NumericVector {(double)pair.first, (double)pair.second};
}

template<>
SEXP Rcpp::wrap(const genomic_interval_t& genomic_interval) {
  Rcpp::List genomic_interval_list = Rcpp::List::create(genomic_interval.contig_name, Rcpp::wrap(genomic_interval.interval));
  genomic_interval_list.names() = Rcpp::CharacterVector({"Contig Name", "Contig Positions"});
  return genomic_interval_list;
}

template<>
SEXP Rcpp::wrap(const std::vector<genomic_field_t>& genomic_fields) {
  Rcpp::List wrapped_genomic_fields(genomic_fields.size());
  for (auto i=0u; i<genomic_fields.size(); i++) {
    Rcpp::CharacterVector field = Rcpp::CharacterVector::create(genomic_fields[i].first, genomic_fields[i].second);
    wrapped_genomic_fields[i] = field;
  }
  return wrapped_genomic_fields;
}

// [[Rcpp::plugins(cpp11)]]

// [[Rcpp::export]]
Rcpp::CharacterVector version() {
  return genomicsdb_version();
}

// [[Rcpp::export]]
Rcpp::XPtr<GenomicsDB> setup(const std::string& workspace,
                             const std::string& vid_mapping_file,
                             const std::string& callset_mapping_file,
                             const std::string& reference_genome,
                             const std::vector<std::string> attributes) {
  GenomicsDB *genomicsdb = new GenomicsDB(workspace, callset_mapping_file, vid_mapping_file, reference_genome, attributes, 40);
  
  Rcpp::Rcout << "Got GenomicsDB" << std::endl;
  return Rcpp::XPtr<GenomicsDB>(genomicsdb);
}

// [[Rcpp::export]]
Rcpp::XPtr<GenomicsDB> setup_from_json(const std::string& query_configuration_json_file, const std::string& loader_configuration_json_file) {
  return  Rcpp::XPtr<GenomicsDB>(new GenomicsDB(query_configuration_json_file, loader_configuration_json_file));
  
}

genomicsdb_ranges_t convert(SEXP ranges_exp) {
  Rcpp::List ranges(ranges_exp);
  genomicsdb_ranges_t results;
  for (int i=0; i<ranges.size(); i++) {
    Rcpp::NumericVector range = ranges[i];
    if (range.size() > 2) {
      throw Rcpp::exception("Range specified in the list has to be either a start/end pair or a single to specify an interval. E.g ranges <- list(c(0, 100), c(200), c(500, 1000)). A single range \"c(200)\" will be interpreted as \"c(200, 200)\" internally");
    }
    if (range.size() == 1) {
      results.push_back(std::make_pair(static_cast<int64_t>(range[0]), static_cast<int64_t>(range[0])));
    } else {
      results.push_back(std::make_pair(static_cast<int64_t>(range[0]), static_cast<int64_t>(range[1])));
    }
  }
  return results;
}

// [[Rcpp::export]]
Rcpp::List query_variants(Rcpp::XPtr<GenomicsDB> genomicsdb,
                 const std::string& array,
                 Rcpp::List column_ranges,
                 Rcpp::List row_ranges) {
  genomicsdb_ranges_t column_ranges_vector = convert(column_ranges);
  genomicsdb_ranges_t row_ranges_vector = convert(row_ranges);

  std::vector<Rcpp::List> variants_vector;
  try {
    GenomicsDBVariants results = genomicsdb.get()->query_variants1(array, column_ranges_vector, row_ranges_vector);
    Rcpp::Rcout << "Number of results returned = " <<  results.size() << std::endl;
    while (auto variant = results.next()) {
      // Interval
      interval_t interval = genomicsdb.get()->get_interval(variant);

      // Genomic Interval
      genomic_interval_t genomic_interval = genomicsdb.get()->get_genomic_interval(variant);

      // Genomic Fields
      std::vector<genomic_field_t> fields = genomicsdb.get()->get_genomic_fields(array, variant);
      /*      Rcpp::List genomic_fields(fields.size());
      for (auto i=0u; i<fields.size(); i++) {
        
      }*/
      
      Rcpp::List variant_list = Rcpp::List::create(interval, genomic_interval, fields);
      variant_list.names() = Rcpp::CharacterVector({"Interval", "Genomic Interval", "Genomic Fields"});
      variants_vector.push_back(variant_list);
    }
    results.free();    
  } catch (const std::exception& e) {
    Rcpp::Rcerr << "GenomicsDB Exception: " << e.what() << "\nquery_variants() aborted!" << std::endl;
  }
  return Rcpp::List::create(variants_vector);
}

