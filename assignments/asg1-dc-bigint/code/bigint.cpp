// $Id: bigint.cpp,v 1.4 2021-09-26 10:35:44-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"

// bigint::bigint (long that): uvalue (ubigint(that)), 
//    is_negative (that < 0) {
//    // Set uvalue to a temp ubigint that will parse and store the 
// digits
//    // of that into a vector
//    // uvalue = ubigint(that);
//    // ubigint temp {that};
//    // uvalue = temp;
//    // DEBUGF ('~', this << " -> " << uvalue)
// }

// bigint::bigint (const ubigint& uvalue_, bool is_negative_):
//                 uvalue(uvalue_), is_negative(is_negative_) {
// }

// bigint::bigint (const string& that) {
//    is_negative = that.size() > 0 and that[0] == '_';
//    uvalue = ubigint (that.substr (is_negative ? 1 : 0));
// }

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
//    DEBUGF ('~', this << " -> " << uvalue)
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
   // Initialize a default bigint result to hold the sum
   bigint bigIntResult;

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

bigint bigint::operator- (const bigint& that) const {
   // Initialize a default bigint result to hold the sum
   bigint bigIntResult;

   // If the signs are different
   if (is_negative != that.is_negative) {
      // Add the uvalues
      bigIntResult.uvalue = uvalue + that.uvalue;
      // Set the sign of the result to the left number
      bigIntResult.is_negative = is_negative;
   } // If the signs are the same
   else if (is_negative == that.is_negative) {
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
      // bigIntResult.uvalue = largerNum.uvalue - smallerNum.uvalue;
      ubigint unsignedResult {largerNum.uvalue - smallerNum.uvalue};
      bigIntResult.uvalue = unsignedResult;

      // If left num is larger, the sign of the result is its sign
      if (largerNum > smallerNum) {
         bigIntResult.is_negative = largerNum.is_negative;
      } // Otherwise, the result has the opposite of the sign of the
      // right number
      else {
         bigIntResult.is_negative = (smallerNum.is_negative) ?
            (false) : (true);
      }
   }

   return bigIntResult;
}


bigint bigint::operator* (const bigint& that) const {
   // Initialize a default bigint result to hold the sum
   bigint bigIntResult;
   
   // Multiply the bigints with ubigint::*
   ubigint unsignedResult {uvalue * that.uvalue};
   bigIntResult.uvalue = unsignedResult;
   // Determine the result's signs:
   // * same sign -> not negative
   // * diff sign -> negative
   bigIntResult.is_negative = (is_negative == that.is_negative) ? 
      (false) : (true);
   
   return bigIntResult;
}

bigint bigint::operator/ (const bigint& that) const {
   // Initialize a default bigint result to hold the sum
   bigint bigIntResult;
   
   // Divide the bigints with ubigint::/
   ubigint unsignedResult {uvalue / that.uvalue};
   bigIntResult.uvalue = unsignedResult;
   // Determine the result's signs:
   // * same sign -> not negative
   // * diff sign -> negative
   bigIntResult.is_negative = (is_negative == that.is_negative) ? 
      (false) : (true);

   return bigIntResult;
   // bigint result {uvalue / that.uvalue};
   // return result;
}

bigint bigint::operator% (const bigint& that) const {
   // Initialize a default bigint result to hold the sum
   bigint bigIntResult;
   
   // Divide the bigints with ubigint::/
   ubigint unsignedResult {uvalue % that.uvalue};
   bigIntResult.uvalue = unsignedResult;
   // Determine the result's signs:
   // * same sign -> not negative
   // * diff sign -> negative
   bigIntResult.is_negative = (is_negative == that.is_negative) ? 
      (false) : (true);

   return bigIntResult;
   // bigint result {uvalue % that.uvalue};
   // return result;
}

bool bigint::operator== (const bigint& that) const {
   // Both conditions need to be true for both bigints to be equal:
   // 1. The signs are the same
   // 2. uvalue == that.uvalue
   return is_negative == that.is_negative && uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   bool output = false;
   // If the left sign is negative and the right side is positive, left
   // is less than right
   if (is_negative == false && that.is_negative == true) {
      return false;
   } else if (is_negative == true && that.is_negative == false) {
      return true;
   } else if (is_negative == false && that.is_negative == false) {
      output = (uvalue < that.uvalue) ? (true) : (false);
   } else if (is_negative == true && that.is_negative == true) {
      output = (uvalue < that.uvalue) ? (false) : (true);
   }

   return output;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << "bigint(" << (that.is_negative ? "-" : "+")
              << "," << that.uvalue << ")";
}
