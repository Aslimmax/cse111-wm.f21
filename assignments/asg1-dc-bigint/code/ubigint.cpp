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
      pairwiseSum = 0; // reset the sum
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
      // while (result.ubig_value.size() > 0 &&
      //  result.ubig_value.back() == 0) {
      //    result.ubig_value.pop_back();
      // }
      return result;
   }

   // store the size of largerVec
   int largerVecSize = largerVec.ubig_value.size();

   // At this point, there are still digits in largerVec
   for (int i = smallerVec; i < largerVecSize; i++) {
      int largerVecDigit = largerVec.ubig_value[i];
      // check if the digit from the previous loop produced a carryover
      if (carryover == 1) {
         largerVecDigit++;
         carryover = 0;
      }
      // pushback the remaining digits in largerVec
      result.ubig_value.push_back(largerVecDigit);
   }
   // DEBUGF ('u', result);
   // while (result.ubig_value.size() > 0 &&
   //  result.ubig_value.back() == 0) {
   //    result.ubig_value.pop_back();
   // }
   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   if (ubig_value.size() < that.ubig_value.size()) {
      throw domain_error ("ubigint::operator-(a<b)");
   }

   ubigint result; // initialize result to store the overall difference
   // store the largerVec to add the remaining digits to result once the
   // smaller one runs out of digits
   ubigint largerVec;
   int pairwiseDiff = 0; // store the pairwise difference of the digits
   int smallerVec = 0; // store the length of the smaller vector
   int carryover = 0; // store the carryover value (0 or 1)
   
   // Get the sizes of each vector
   int leftVecSize = ubig_value.size();
   int rightVecSize = that.ubig_value.size();
   
//    // Determine smaller vector
//    if (leftVecSize < rightVecSize) {
//       smallerVec = leftVecSize;
//    } else if (leftVecSize > rightVecSize) {
//       smallerVec = rightVecSize;
//    } else { // the sizes are equal, doesn't matter which one is set
//       smallerVec = leftVecSize;
//    }

   // Loop through vectors and add each digit pairwise
   for (int i = 0; i < rightVecSize; i++) {
      pairwiseDiff = 0; // reset the difference
      // if the left digit is less than the right digit, borrowing will
      // occur. Set pairwiseDiff to 10 to mimic adding 10 to the left
      // digit
      if (ubig_value[i] < that.ubig_value[i]) {
         pairwiseDiff = 10;
      }
      
      pairwiseDiff -= ubig_value[i] - that.ubig_value[i] - carryover;

      // check if carryover is needed
      carryover = (ubig_value[i] < that.ubig_value[i]) ? (1) : (0);

      // pushback pairwiseSum
      result.ubig_value.push_back(pairwiseDiff);
   }

   // Loop through the rest of the digits remaining in this ubig_value
   for (int i = rightVecSize; i < leftVecSize; i++) {
      int largerVecDigit = largerVec.ubig_value[i];
      // check if the digit from the previous loop produced a carryover
      if (carryover == 1) {
         largerVecDigit--;
         carryover = 0;
      }
      // pushback the remaining digits in largerVec
      result.ubig_value.push_back(largerVecDigit);
   }
   while (result.ubig_value.size() > 0 &&
    result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }
   return result;
   // return ubigint (uvalue - that.uvalue);
}

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

bool ubigint::operator== (const ubigint& that) const {
   // Validate that the size of the vectors are the same
   if (ubig_value.size() != that.ubig_value.size()) {
      return false;
   }

   // At this point, both vectors have the same size
   int vecSize = ubig_value.size(); // get the size of one vector
   // Loop through both vectors and compare each digit
   for (int i = 0; i < vecSize; i++) {
      if (ubig_value[i] != that.ubig_value[i]) {
         return false;
      }
   }

   // Each digit in both vectors are the same
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   // Store the sizes of each vector
   int leftVecSize = ubig_value.size();
   int rightVecSize = that.ubig_value.size();

   // Validate that the size of the vectors are the same
   if (leftVecSize < rightVecSize) {
      // the left vec has more digits
      return true;
   } else if (leftVecSize > rightVecSize) { 
      // the right vec has more digits
      return false;
   }

   // Both vectors have the same size, so compare each digits starting
   // from the high order end to the low order end
   for (int i = 0; i < leftVecSize; i++) {
      // if any digit in the leftVec is less than the rightVec, then
      // leftVec < rightVec
      if (ubig_value[leftVecSize - i - 1] < 
      that.ubig_value[leftVecSize - i - 1]) {
         return true;
      }
      // if any digit in the leftVec is greater than the rightVec, then
      // leftVec > rightVec 
      else if (ubig_value[leftVecSize - i - 1] > 
      that.ubig_value[leftVecSize - i - 1]) {
         return false;
      }
   }

   // At this point, no difference was found between the two vectors
   return false;
}

// ostream& operator<< (ostream& out, const ubigint& that) { 
//    return out << "ubigint(" << that.uvalue << ")";
// }

