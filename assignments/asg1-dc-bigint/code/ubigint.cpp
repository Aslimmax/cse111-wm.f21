// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <string>
#include <stdexcept>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

ubigint::ubigint (unsigned long that): ubig_value () {
   // store the new end digit every time 'that' is truncated
   int digit = 0; 
   
   while(that != 0) { // don't exit loop until 'that' is 0
      digit = that % 10; // get the last digit in 'that'
      ubig_value.push_back(digit); // store the digit into ubig_value
      that = that / 10; // delete the last digit in 'that'
   }
//    DEBUGF ('~', this << " -> " << uvalue)

}

ubigint::ubigint (const string& that): ubig_value() {
   DEBUGF ('~', "that = \"" << that << "\"");
   // Iterate through the string starting from the end
   for (auto iter = that.crbegin(); iter != that.crend(); ++iter) {
      // Push each digit to the end of ubig_value
      ubig_value.push_back(*iter - '0');
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   // DEBUGF ('u', *this << "+" <`< that);
   ubigint result; // initialize result to store the result of adding
   // store the largerVec to add the remaining digits to result once the
   // smaller one runs out of digits
   ubigint largerVec;
   int pairwiseSum = 0; // store the pairwise sum of the two numbers
   int smallerVec = 0; // store the length of the smaller vector
   int carryover = 0; // store the carryover value (0 or 1)

   // Get the sizes of each vector
   int leftVecSize = ubig_value.size();
   int rightVecSize = that.ubig_value.size();
   
   // Determine smaller vector
   if (leftVecSize < rightVecSize) {
      smallerVec = leftVecSize;
   } else if (leftVecSize > rightVecSize) {
      smallerVec = rightVecSize;
   } else { // the sizes are equal, doesn't matter which one is set
      smallerVec = leftVecSize;
   }

   // Loop through vectors and add each digit pairwise
   for (int i = 0; i < smallerVec; i++) {
      pairwiseSum = ubig_value[i] + that.ubig_value[i] + carryover;

      // check if carryover is needed
      carryover = (pairwiseSum >= 10) ? (1) : (0);

      // pushback pairwiseSum
      result.ubig_value.push_back(pairwiseSum);
   }

   // Determine which vector still has digits to add
   if (leftVecSize < rightVecSize) {
      largerVec.ubig_value = that.ubig_value;
   } else if (leftVecSize > rightVecSize) {
      largerVec.ubig_value = ubig_value;
   } else { // both vectors had the same num of digits
      return result;
   }

   // At this point, there are still digits in largerVec
   for (int i = smallerVec; i < largerVec.ubig_value.size(); i++) {
      // pushback the remaining digits in largerVec
      result.ubig_value.push_back(largerVec.ubig_value.size());
   }
   // DEBUGF ('u', result);
   return result;
}

// ubigint ubigint::operator- (const ubigint& that) const {
//    if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
//    return ubigint (uvalue - that.uvalue);
// }

// ubigint ubigint::operator* (const ubigint& that) const {
//    return ubigint (uvalue * that.uvalue);
// }

// void ubigint::multiply_by_2() {
//    uvalue *= 2;
// }

// void ubigint::divide_by_2() {
//    uvalue /= 2;
// }

// 
// struct quo_rem { ubigint quotient; ubigint remainder; };
// quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
//    // NOTE: udivide is a non-member function.
//    ubigint divisor {divisor_};
//    ubigint zero {0};
//    if (divisor == zero) throw domain_error ("udivide by zero");
//    ubigint power_of_2 {1};
//    ubigint quotient {0};
//    ubigint remainder {dividend}; // left operand, dividend
//    while (divisor < remainder) {
//       divisor.multiply_by_2();
//       power_of_2.multiply_by_2();
//    }
//    while (power_of_2 > zero) {
//       if (divisor <= remainder) {
//          remainder = remainder - divisor;
//          quotient = quotient + power_of_2;
//       }
//       divisor.divide_by_2();
//       power_of_2.divide_by_2();
//    }
//    DEBUGF ('/', "quotient = " << quotient);
//    DEBUGF ('/', "remainder = " << remainder);
//    return {.quotient = quotient, .remainder = remainder};
// }

// ubigint ubigint::operator/ (const ubigint& that) const {
//    return udivide (*this, that).quotient;
// }

// ubigint ubigint::operator% (const ubigint& that) const {
//    return udivide (*this, that).remainder;
// }

// bool ubigint::operator== (const ubigint& that) const {
//    return uvalue == that.uvalue;
// }

// bool ubigint::operator< (const ubigint& that) const {
//    return uvalue < that.uvalue;
// }

// ostream& operator<< (ostream& out, const ubigint& that) { 
//    return out << "ubigint(" << that.uvalue << ")";
// }

