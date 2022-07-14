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
                               const std::vector<std::string> attributes = {},
                               const uint64_t segment_size = 10*1024*1024) {
  GenomicsDB *genomicsdb = new GenomicsDB(workspace, callset_mapping_file, vid_mapping_file, attributes, segment_size);
  
  Rcpp::Rcout << "Got GenomicsDB" << std::endl;
  return Rcpp::XPtr<GenomicsDB>(genomicsdb);
}

// [[Rcpp::export]]
Rcpp::XPtr<GenomicsDB> connect_with_query_json(const std::string& query_configuration_json_file,
                                               const int query_configuration_type = 0, // GenomicsDB::JSON_FILE 
                                               const std::string& loader_configuration_json_file = "",
                                               const int concurrency_rank = 0) {
  return  Rcpp::XPtr<GenomicsDB>(new GenomicsDB(query_configuration_json_file, (GenomicsDB::query_config_type_t)query_configuration_type, loader_configuration_json_file, concurrency_rank)); 
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

#define STRING_FIELD(NAME, TYPE) (TYPE.is_string() || TYPE.is_char() || TYPE.num_elements > 1 || (NAME.compare("GT") == 0))
#define INT_FIELD(TYPE) (TYPE.is_int())
#define FLOAT_FIELD(TYPE) (TYPE.is_float())
class ColumnarVariantCallProcessor : public GenomicsDBVariantCallProcessor {
 public:
  void process(const interval_t& interval) {
    if (!m_is_initialized) {
      m_is_initialized = true;
      const std::shared_ptr<std::map<std::string, genomic_field_type_t>> genomic_field_types = get_genomic_field_types();
      for (std::pair<std::string, genomic_field_type_t> field_type_pair : *genomic_field_types) {
        std::string field_name = field_type_pair.first;
        genomic_field_type_t field_type = field_type_pair.second;
        if (field_name.compare("END")==0) {
          continue;
        }
        // Order fields by inserting REF and ALT in the beginning
        if (!field_name.compare("REF") && m_field_names.size() > 1) {
          m_field_names.insert(m_field_names.begin(), field_name);
        } else if (!field_name.compare("ALT") && m_field_names.size() > 2) {
          m_field_names.insert(m_field_names.begin()+1, field_name);
        } else {
          m_field_names.push_back(field_name);
        }
        if (STRING_FIELD(field_name, field_type)) {
          m_string_fields.emplace(std::make_pair(field_name, std::shared_ptr<std::vector<std::string>>(new std::vector<std::string>())));
        } else if (INT_FIELD(field_type)) {
          m_int_fields.emplace(std::make_pair(field_name, std::shared_ptr<std::vector<int>>(new std::vector<int>())));
        } else if (FLOAT_FIELD(field_type)) {
           m_float_fields.emplace(std::make_pair(field_name, std::shared_ptr<std::vector<float>>(new std::vector<float>())));
        } else {
          std::string msg = "Genomic field type for " + field_name + " not supported";
          throw Rcpp::exception(msg.c_str());
        }
      }
    }
  }

  void process_fields(const std::vector<genomic_field_t>& genomic_fields) {
    for (auto field_name: m_field_names) {
      // END is part of the Genomic Coordinates, so don't process here
      if (field_name.compare("END") == 0) {
        continue;
      }

      auto field_type = get_genomic_field_types()->at(field_name);
      
      bool found = false;
      for (auto genomic_field: genomic_fields) {
        if (genomic_field.name.compare(field_name) == 0) {
          if (STRING_FIELD(field_name, field_type)) {
            m_string_fields[field_name]->push_back(genomic_field.to_string(field_type));
          } else if (INT_FIELD(field_type)) {
            m_int_fields[field_name]->push_back(genomic_field.int_value_at(0));
          } else if (FLOAT_FIELD(field_type)) {
            m_float_fields[field_name]->push_back( genomic_field.float_value_at(0));
          } else {
            std::string msg = "Genomic field type for " + field_name + " not supported";
            throw Rcpp::exception(msg.c_str());
          }
          found = true;
          break;
        }
      }

      if (!found) {
        if (STRING_FIELD(field_name, field_type)) {
          m_string_fields[field_name]->push_back("");
        } else if (INT_FIELD(field_type)) {
          m_int_fields[field_name]->push_back(Rcpp::IntegerVector::get_na());
        } else if (FLOAT_FIELD(field_type)) {
          m_float_fields[field_name]->push_back(Rcpp::NumericVector::get_na());
        } else {
          std::string msg = "Genomic field type for " + field_name + " not supported";
          throw Rcpp::exception(msg.c_str());
        }
      }
    }
  }

  void process(const std::string& sample_name,
               const int64_t* coordinates,
               const genomic_interval_t& genomic_interval,
               const std::vector<genomic_field_t>& genomic_fields) {
    m_rows.push_back(coordinates[0]);
    m_cols.push_back(coordinates[1]);
    m_sample_names.push_back(sample_name);
    m_chrom.push_back(genomic_interval.contig_name);
    m_pos.push_back(genomic_interval.interval.first);
    m_end.push_back(genomic_interval.interval.second);
    process_fields(genomic_fields);
  }

  Rcpp::DataFrame get_data_frame() {
    auto num_fixed_fields = 6u;
    
    Rcpp::CharacterVector names(num_fixed_fields+m_field_names.size());
    names[0] = "ROW";
    names[1] = "COL";
    names[2] = "SAMPLE";
    names[3] = "CHROM";
    names[4] = "POS";
    names[5] = "END";
    
    Rcpp::List vector_list(num_fixed_fields+m_field_names.size());
    vector_list[0] = m_rows;
    vector_list[1] = m_cols;
    vector_list[2] = m_sample_names;
    vector_list[3] = m_chrom;
    vector_list[4] = m_pos;
    vector_list[5] = m_end;

    auto i = num_fixed_fields-1;
    for (auto field_name: m_field_names) {
      names[++i] = field_name;
      if (m_string_fields.find(field_name) != m_string_fields.end()) {
        vector_list[i] = *m_string_fields[field_name];
      } else if (m_int_fields.find(field_name) != m_int_fields.end()) {
        vector_list[i] = *m_int_fields[field_name];
      } else if (m_float_fields.find(field_name) != m_float_fields.end()) {
        vector_list[i] = *m_float_fields[field_name];
      } else {
        std::string msg = "Genomic field type for " + field_name + " not supported";
        throw Rcpp::exception(msg.c_str());
      }
    }

    Rcpp::DataFrame data_frame(vector_list);
    data_frame.attr("names") = names;
    
    return data_frame;
  }

 protected:
  bool m_is_initialized = false;
  std::vector<int64_t> m_rows;
  std::vector<int64_t> m_cols;
  std::vector<std::string> m_sample_names;
  std::vector<std::string> m_chrom;
  std::vector<uint64_t> m_pos;
  std::vector<uint64_t> m_end;
  std::vector<std::string> m_field_names;
  std::map<std::string, std::shared_ptr<std::vector<std::string>>> m_string_fields;
  std::map<std::string, std::shared_ptr<std::vector<int>>> m_int_fields;
  std::map<std::string, std::shared_ptr<std::vector<float>>> m_float_fields;
};

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
Rcpp::DataFrame query_variant_calls(Rcpp::XPtr<GenomicsDB> genomicsdb,
                 const std::string& array,
                 Rcpp::List column_ranges,
                 Rcpp::List row_ranges) {
  genomicsdb_ranges_t column_ranges_vector = convert(column_ranges);
  genomicsdb_ranges_t row_ranges_vector = convert(row_ranges);

  ColumnarVariantCallProcessor processor;
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
  return processor.get_data_frame();
}

// [[Rcpp::export]]
Rcpp::DataFrame query_variant_calls_json(Rcpp::XPtr<GenomicsDB> genomicsdb) {
  VariantCallProcessor processor;
  try {
    GenomicsDBVariantCalls results = genomicsdb.get()->query_variant_calls(processor);
    // TBD: As of now query_variant_calls does not return any GenomicsDBVariantCalls.
    //      All results are returned via the registered VariantCallProcessor process() callbacks
    if (results.size() != 0) {
      throw std::logic_error("Not yet implemented. query_variant_calls is not expected to return results");
    }
  } catch (const std::exception& e) {
    std::string msg = std::string(e.what()) + "\nquery_variant_calls_json() aborted!";
    throw Rcpp::exception(msg.c_str());
  }
  return processor.get_intervals();
}

// [[Rcpp::export]]
Rcpp::List query_variant_calls_by_interval(Rcpp::XPtr<GenomicsDB> genomicsdb,
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
    std::string msg = std::string(e.what()) + "\nquery_variant_calls_by_interval() aborted!";
    throw Rcpp::exception(msg.c_str());
  }
  return processor.get_intervals();
}


// [[Rcpp::export]]
void generate_vcf(Rcpp::XPtr<GenomicsDB> genomicsdb,
                  const std::string& array,
                  Rcpp::List column_ranges,
                  Rcpp::List row_ranges,
                  const std::string& reference_genome,
                  const std::string& vcfheader_template,
                  const std::string& output,
                  const std::string& output_format,
                  bool overwrite) {
  genomicsdb_ranges_t column_ranges_vector = convert(column_ranges);
  genomicsdb_ranges_t row_ranges_vector = convert(row_ranges);
  genomicsdb.get()->generate_vcf(array, column_ranges_vector, row_ranges_vector, reference_genome, vcfheader_template, output, output_format, overwrite);
}



// [[Rcpp::export]]
void generate_vcf_json(Rcpp::XPtr<GenomicsDB> genomicsdb,
                  const std::string& output,
                  const std::string& output_format,
                  bool overwrite) {
  genomicsdb.get()->generate_vcf(output, output_format, overwrite);
}

