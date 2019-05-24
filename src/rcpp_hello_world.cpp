
#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
List rcpp_hello_world() {

    CharacterVector x = CharacterVector::create( "foo", "bar" )  ;
    NumericVector y   = NumericVector::create( 0.0, 1.0 ) ;
    List z            = List::create( x, y ) ;

    return z ;
}


// [[Rcpp::export]]
void rcpp_vector_access1(){

  // Creating vector
  NumericVector v  {10,20,30,40,50};

  Rcpp::Rcout << v << std::endl;

  // Setting element names
  v.names() = CharacterVector({"A","B","C","D","E"});

  Rcpp::Rcout << v << std::endl;

  // Preparing vector for access
  NumericVector   numeric = {1,3};
  IntegerVector   integer = {1,3};
  CharacterVector character = {"B","D"};
  LogicalVector   logical = {false, true, false, true, false};

  // Getting values of vector elements
  NumericVector res1 = v[numeric];
  NumericVector res2 = v[integer];
  NumericVector res3 = v[character];
  NumericVector res4 = v[logical];

  // Assigning values to vector elements
  v[0]   = 100;
  v["A"] = 100;
  NumericVector v2 {100,200};
  v[numeric]   = v2;
  v[integer]   = v2;
  v[character] = v2;
  v[logical]   = v2;

  Rcpp::Rcout << v << std::endl;
}
