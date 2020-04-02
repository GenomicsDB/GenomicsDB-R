/**
 * @file genomicsdb_query.cpp
 *
 * @section LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Omics Data Automation, Inc.
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

/*template<>
SEXP Rcpp::wrap(const std::pair<const std::vector<genomic_field_t>&, const std::map<std::string, genomic_field_type_t>&> genomic_fields_with_types) {
  auto genomic_fields = genomic_fields_with_types.first;
  auto genomic_field_types = genomic_fields_with_types.second;
  Rcpp::List wrapped_genomic_fields(genomic_fields.size());
  for (auto i=0u; i<genomic_fields.size(); i++) {
    auto field_name = genomic_fields[i].name;
    Rcpp::CharacterVector field = Rcpp::CharacterVector::create(field_name, genomic_fields[i].to_string(genomic_field_types.at(field_name)));
    wrapped_genomic_fields[i] = field;
  }
  return wrapped_genomic_fields;
  }*/

// [[Rcpp::plugins(cpp11)]]

// [[Rcpp::export]]
Rcpp::CharacterVector version() {
  return genomicsdb_version();
}

// [[Rcpp::export]]
Rcpp::XPtr<GenomicsDB> connect(const std::string& workspace,
                               const std::string& vid_mapping_file,
                               const std::string& callset_mapping_file,
                               const std::string& reference_genome,
                               const std::vector<std::string> attributes,
                               const uint64_t segment_size = 10*1024*1024) {
  GenomicsDB *genomicsdb = new GenomicsDB(workspace, callset_mapping_file, vid_mapping_file, reference_genome, attributes, segment_size);
  
  Rcpp::Rcout << "Got GenomicsDB" << std::endl;
  return Rcpp::XPtr<GenomicsDB>(genomicsdb);
}

// [[Rcpp::export]]
Rcpp::XPtr<GenomicsDB> connect_with_query_json(const std::string& query_configuration_json_file,
                                               const std::string& loader_configuration_json_file,
                                               const int concurrency_rank = 0) {
  return  Rcpp::XPtr<GenomicsDB>(new GenomicsDB(query_configuration_json_file, GenomicsDB::JSON_FILE, loader_configuration_json_file, concurrency_rank)); 
}

// [[Rcpp::export]]
void disconnect(Rcpp::XPtr<GenomicsDB> genomicsdb) {
  // Garbage collected automatically by Rcpp for now
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
    GenomicsDBResults<genomicsdb_variant_t> results = genomicsdb.get()->query_variants(array, column_ranges_vector, row_ranges_vector);
    while (auto variant = results.next()) {
      // Interval
      interval_t interval = genomicsdb.get()->get_interval(variant);

      // Genomic Interval
      genomic_interval_t genomic_interval = genomicsdb.get()->get_genomic_interval(variant);

      // Genomic Fields
      std::vector<genomic_field_t> fields = genomicsdb.get()->get_genomic_fields(array, variant);

      // Variant Calls
      GenomicsDBVariantCalls calls = genomicsdb.get()->get_variant_calls(array, variant);
      Rcpp::Rcout << "Number of calls returned = " <<  calls.size() << std::endl;

      Rcpp::List wrapped_genomic_fields(calls.size());
      auto i=0ul;
      for (auto field: fields) {
        Rcpp::CharacterVector field_vec = Rcpp::CharacterVector::create(field.name, field.to_string(calls.get_genomic_field_type(field.name)));
        wrapped_genomic_fields[i++] = field_vec;
      }

      Rcpp::List variant_list = Rcpp::List::create(interval, genomic_interval, wrapped_genomic_fields);
      variant_list.names() = Rcpp::CharacterVector({"Interval", "Genomic Interval", "Genomic Fields"});
      variants_vector.push_back(variant_list);
    }
  } catch (const std::exception& e) {
    std::string msg = std::string(e.what()) + "\nquery_variants() aborted!";
    throw Rcpp::exception(msg.c_str());
  }
  return Rcpp::List::create(variants_vector);
}

class VariantCallProcessor : public GenomicsDBVariantCallProcessor {
 public:
  void finalize_current_interval() {
    if (m_variant_calls_map.size() > 0) {
      std::vector<Rcpp::List> variant_calls_vector;
      for (std::map<uint64_t, std::vector<Rcpp::List>>::iterator it=m_variant_calls_map.begin(); it!=m_variant_calls_map.end(); ++it) {
        Rcpp::List variant_call_list = Rcpp::List::create(it->first, wrap(it->second));
        variant_call_list.names() = Rcpp::CharacterVector({"Row", "Info"});
        variant_calls_vector.push_back(variant_call_list);
      }
      Rcpp::List variant_call_list_for_interval = Rcpp::List::create(m_interval, wrap(variant_calls_vector));
      variant_call_list_for_interval.names() = Rcpp::CharacterVector({"Query Interval", "Variant Calls"});
      m_intervals_vector.push_back(variant_call_list_for_interval);
      m_variant_calls_map.clear();
    }
  }

  void process(const interval_t& interval) {
    // GenomicsDBVariantCallProcessor::process(interval);
    finalize_current_interval();
    m_interval = interval;
  }

  Rcpp::List process_genomic_fields(const std::vector<genomic_field_t>& genomic_fields) {
    Rcpp::List wrapped_genomic_fields(genomic_fields.size());
    auto i=0ul;
    for (auto field: genomic_fields) {
      Rcpp::CharacterVector field_vec = Rcpp::CharacterVector::create(field.name, field.to_string(get_genomic_field_types()->at(field.name)));
      wrapped_genomic_fields[i++] = field_vec;
    }
    return wrapped_genomic_fields;
  }

  void process(const std::string& sample_name,
               const int64_t* coordinates,
               const genomic_interval_t& genomic_interval,
               const std::vector<genomic_field_t>& genomic_fields) {
    Rcpp::List variant_call_list = Rcpp::List::create(genomic_interval, process_genomic_fields(genomic_fields));
    variant_call_list.names() = Rcpp::CharacterVector({"Genomic Interval", "Genomic Fields"});
    auto row = coordinates[0];
    auto it = m_variant_calls_map.find(row);
    if (it != m_variant_calls_map.end()) {
      it->second.push_back(variant_call_list);
    } else {
      m_variant_calls_map.emplace(row, std::vector<Rcpp::List> { variant_call_list } );
    }
  }

  Rcpp::List get_intervals() {
    finalize_current_interval();
    return Rcpp::List::create(m_intervals_vector);
  }

  interval_t m_interval;
  std::vector<Rcpp::List> m_intervals_vector;
  std::map<uint64_t, std::vector<Rcpp::List>> m_variant_calls_map;
};

// [[Rcpp::export]]
Rcpp::List query_variant_calls_json(Rcpp::XPtr<GenomicsDB> genomicsdb) {
  VariantCallProcessor processor;
  try {
    GenomicsDBVariantCalls results = genomicsdb.get()->query_variant_calls(processor);
    // TBD: As of now query_variant_calls does not return any GenomicsDBVariantCalls.
    //      All results are returned via the registered VariantCallProcessor process() callbacks
    if (results.size() != 0) {
      throw std::logic_error("Not yet implemented. query_variant_calls is not expected to return results");
    }
  } catch (const std::exception& e) {
    std::string msg = std::string(e.what()) + "\nquery_variant_calls() aborted!";
    throw Rcpp::exception(msg.c_str());
  }
  return processor.get_intervals();
}

// [[Rcpp::export]]
Rcpp::List query_variant_calls(Rcpp::XPtr<GenomicsDB> genomicsdb,
                 const std::string& array,
                 Rcpp::List column_ranges,
                 Rcpp::List row_ranges) {
  genomicsdb_ranges_t column_ranges_vector = convert(column_ranges);
  genomicsdb_ranges_t row_ranges_vector = convert(row_ranges);

  VariantCallProcessor processor;
  try {
    GenomicsDBVariantCalls results = genomicsdb.get()->query_variant_calls(processor, array, column_ranges_vector, row_ranges_vector);
    // TBD: As of now query_variant_calls does not return any GenomicsDBVariantCalls. 
    //      All results are returned via the registered VariantCallProcessor process() callbacks
    if (results.size() != 0) {
      throw std::logic_error("Not yet implemented. query_variant_calls is not expected to return results");
    }
  } catch (const std::exception& e) {
    std::string msg = std::string(e.what()) + "\nquery_variant_calls() aborted!";
    throw Rcpp::exception(msg.c_str());
  }
  return processor.get_intervals();
}

// [[Rcpp::export]]
void generate_vcf(Rcpp::XPtr<GenomicsDB> genomicsdb,
                  const std::string& output,
                  const std::string& output_format,
                  bool overwrite) {
  genomicsdb.get()->generate_vcf(output, output_format, overwrite);
}
