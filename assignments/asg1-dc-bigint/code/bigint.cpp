// $Id: bigint.cpp,v 1.4 2021-09-26 10:35:44-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"

bigint::bigint (long that): uvalue (ubigint(that)), is_negative (that < 0) {
   // Set uvalue to a temp ubigint that will parse and store the digits
   // of that into a vector
   // uvalue = ubigint(that);
   // ubigint temp {that};
   // uvalue = temp;
   // DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

// Make the bigint postive
bigint bigint::operator+ () const {
   return *this;
}

// Makes the bigint negative
bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   // Initialize a default bigint and ubigint result var to hold the sum
   bigint bigIntResult;
   // ubigint uResult;

   // If the signs are the same
   if (is_negative == that.is_negative) {
      // Add the two values with ubigint+
      ubigint unsignedResult {uvalue + that.uvalue};

      // Set the uvalue and is_negative of bigIntResult to 
      // unsignedResult and the sign of either bigint number 
      // respectively
      bigIntResult.uvalue = unsignedResult;
      bigIntResult.is_negative = is_negative;
   } // If the signs are different 
   else if (is_negative != that.is_negative) {
      // Initialize empty bigints to store the larger and smaller
      // bigints
      bigint largerNum; 
      bigint smallerNum;
      // Determine which uvalue is bigger
      if (uvalue < that.uvalue) {
         largerNum = that;
         smallerNum = *this;
      } else if (uvalue > that.uvalue) {
         largerNum = *this;
         smallerNum = that;
      }

      // Subtract the two bigints and use the larger num as its left num
      ubigint unsignedResult {largerNum.uvalue - smallerNum.uvalue};
      
      // Set the uvalue and is_negative of bigIntResult to
      // unsignedResult and the sign of the largerNumber
      bigIntResult.uvalue = unsignedResult;
      bigIntResult.is_negative = largerNum.is_negative;
   }

   return bigIntResult;
}

// bigint bigint::operator- (const bigint& that) const {
//    ubigint result {uvalue - that.uvalue};
//    return result;
// }

// 
// bigint bigint::operator* (const bigint& that) const {
//    bigint result {uvalue * that.uvalue};
//    return result;
// }

// bigint bigint::operator/ (const bigint& that) const {
//    bigint result {uvalue / that.uvalue};
//    return result;
// }

// bigint bigint::operator% (const bigint& that) const {
//    bigint result {uvalue % that.uvalue};
//    return result;
// }

// bool bigint::operator== (const bigint& that) const {
//    return is_negative == that.is_negative and uvalue == that.uvalue;
// }

// bool bigint::operator< (const bigint& that) const {
//    if (is_negative != that.is_negative) return is_negative;
//    return is_negative ? uvalue > that.uvalue
//                       : uvalue < that.uvalue;
// }

ostream& operator<< (ostream& out, const bigint& that) {
   return out << "bigint(" << (that.is_negative ? "-" : "+")
              << "," << that.uvalue << ")";
}

