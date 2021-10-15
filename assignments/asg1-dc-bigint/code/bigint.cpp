// $Id: bigint.cpp,v 1.5 2021-10-14 02:10:43-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"

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

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   // Flip the sign of is_negative
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
      // If unsignedResult is 0, ensure that the sign is positive
      if (unsignedResult.getUBigValue().size() == 0) {
         bigIntResult.is_negative = false;
      }
      else {
          bigIntResult.is_negative = is_negative;
      }

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
      } else if (!(uvalue > that.uvalue)) {
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
   
   // If unsignedResult is 0, ensure that the sign is positive
   if (bigIntResult.uvalue.getUBigValue().size() == 0) {
      bigIntResult.is_negative = false;
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
      bigIntResult.uvalue = largerNum.uvalue - smallerNum.uvalue;

      // If left num is larger, the sign of the result is its sign
      if (that.uvalue < uvalue) {
         bigIntResult.is_negative = is_negative;
      } // Otherwise, the result has the opposite of the sign of the
      // right number
      else {
         bigIntResult.is_negative = (that.is_negative) ?
            (false) : (true);
      }
   }

   // If unsignedResult is 0, ensure that the sign is positive
   if (bigIntResult.uvalue.getUBigValue().size() == 0) {
      bigIntResult.is_negative = false;
   }

   return bigIntResult;
}

bigint bigint::operator* (const bigint& that) const {
   // Initialize a default bigint result to hold the sum
   bigint bigIntResult;
   
   // Multiply the bigints with ubigint::*
   bigIntResult.uvalue = uvalue * that.uvalue;
   // Determine the result's signs:
   // * same sign -> not negative
   // * diff sign -> negative

   if (bigIntResult.uvalue.getUBigValue().size() == 0) {
      bigIntResult.is_negative = false;
   } else {
      bigIntResult.is_negative = (is_negative == that.is_negative) ? 
         (false) : (true);
   }
   return bigIntResult;
}

bigint bigint::operator/ (const bigint& that) const {
   // Initialize a default bigint result to hold the sum
   bigint bigIntResult;
   
   // Divide the bigints with ubigint::/
   // ubigint unsignedResult {uvalue / that.uvalue};
   bigIntResult.uvalue = uvalue / that.uvalue;
   // Determine the result's signs:
   // * same sign -> not negative
   // * diff sign -> negative
   bigIntResult.is_negative = (is_negative == that.is_negative) ? 
      (false) : (true);

   return bigIntResult;
}

bigint bigint::operator% (const bigint& that) const {
   // Initialize a default bigint result to hold the sum
   bigint bigIntResult;
   
   // Divide the bigints with ubigint::/
   // ubigint unsignedResult {uvalue % that.uvalue};
   // bigIntResult.uvalue = unsignedResult;
   bigIntResult.uvalue = uvalue % that.uvalue;
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
   // initialize empty output string to build up the number
   string output = "";
   // Initialize digit and num to store the converted digit
   int digitInt = 0;
   string digitString = "";
   int j = 0;

   // Get size of vector
   int vecSize = that.uvalue.getUBigValue().size();
   
   // If size == 0, print 0
   if (vecSize == 0) {
      output.insert(0, "0");
      if (that.is_negative) {
         output.insert(0, "-");
      }
      return out << output;
   }

   // Check to see if the number is negative
   if (that.is_negative) {
      j++;
      output += "-";
   }

   // Loop through vector and build up output string from the low end
   for (int i = vecSize - 1; i >= 0; i--) {
      // Cast the digit to an int, then to a string
      digitInt = static_cast<int>(that.uvalue.getUBigValue()[i]);
      digitString = to_string(digitInt);
      // Determine the sign of the number      
      if (j % 69 == 0 && j != 0) {
         // cout << "vecSize: " << vecSize << " i: " << i << endl;
         output += "\\";
         output += "\n";
      }
      // Append digit to output string
      output += digitString;
      j++;
   }

   return out << output;
}
