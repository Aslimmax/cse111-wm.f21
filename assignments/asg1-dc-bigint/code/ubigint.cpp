// $Id: ubigint.cpp,v 1.17 2021-10-14 23:59:59-07 - - $

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
   // DEBUGF ('~', this << " -> " << uvalue)
   // store the new end digit every time 'that' is truncated
   int digit = 0; 
   
   while(that != 0) { // don't exit loop until 'that' is 0
      digit = that % 10; // get the last digit in 'that'
      ubig_value.push_back(digit); // store the digit into ubig_value
      that = that / 10; // delete the last digit in 'that'
   }

   // Trim high-order zeros
   while (ubig_value.size() > 0 &&
      ubig_value.back() == 0) {
      ubig_value.pop_back();
   }
}

ubigint::ubigint (const string& that): ubig_value() {
   // DEBUGF ('~', "that = \"" << that << "\"");
   // Iterate through the string starting from the end
   for (auto iter = that.crbegin(); iter != that.crend(); ++iter) {
      // Push each digit to the end of ubig_value
      ubig_value.push_back(*iter - '0');
   }

   // Trim high-order zeros
   while (ubig_value.size() > 0 &&
      ubig_value.back() == 0) {
      ubig_value.pop_back();
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
      
      // Compute the sum with carryover (if defined)
      pairwiseSum = ubig_value[i] + that.ubig_value[i] + carryover;

      // check if carryover is needed
      carryover = (pairwiseSum >= 10) ? (1) : (0);

      // pushback pairwiseSum
      result.ubig_value.push_back(pairwiseSum % 10);
   }

   // Determine which vector still has digits to add
   if (leftVecSize < rightVecSize) {
      largerVec.ubig_value = that.ubig_value;
   } else if (leftVecSize > rightVecSize) {
      largerVec.ubig_value = ubig_value;
   } else { // both vectors had the same num of digits
      // If last sum had carryover, push back the one
      if (carryover == 1) {
         result.ubig_value.push_back(carryover);
      }

      // Trim high order zeros
      while (result.ubig_value.size() > 0 &&
       result.ubig_value.back() == 0) {
         result.ubig_value.pop_back();
      }

      return result;
   }

   // store the size of largerVec
   int largerVecSize = largerVec.ubig_value.size();

   // At this point, there are still digits in largerVec
   for (int i = smallerVec; i < largerVecSize; i++) {
      int largerVecDigit = largerVec.ubig_value[i] + carryover;
      // check if carryover is needed
      carryover = (largerVecDigit >= 10) ? (1) : (0);

      // pushback the remaining digits in largerVec
      result.ubig_value.push_back(largerVecDigit % 10);
   }

   // If last sum had carryover, push back the one   
   if (carryover == 1) {
      result.ubig_value.push_back(1);
   }

   // DEBUGF ('u', result);

   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   // If the left vectory is smaller than the right, throw a domain
   // error
   if (ubig_value.size() < that.ubig_value.size()) {
      throw domain_error ("ubigint::operator-(a<b)");
   }

   ubigint result; // initialize result to store the overall difference
   // store the largerVec to add the remaining digits to result once the
   // smaller one runs out of digits
   int pairwiseDiff = 0; // store the pairwise difference of the digits
   int borrow = 0; // store the borrowing amount
   int carryover = 0; // store the carryover value (0 or 1)
   
   // Get the sizes of each vector
   int leftVecSize = ubig_value.size();
   int rightVecSize = that.ubig_value.size();
   
   // Loop through vectors and subtract each digit pairwise
   for (int i = 0; i < rightVecSize; i++) {
      pairwiseDiff = 0; // reset the difference
      // if the left digit is less than the right digit, borrowing will
      // occur. Set pairwiseDiff to 10 to mimic adding 10 to the left
      // digit
      if (ubig_value[i] - carryover < that.ubig_value[i]) {
         borrow = 10;
      } else {
         borrow = 0;
      }

      // Compute the difference with carryover and borrow (if defined)
      pairwiseDiff = ubig_value[i] - that.ubig_value[i] - carryover 
         + borrow;

      // Check if carryover is needed
      carryover = (borrow == 10) ? (1) : (0);

      // Push back pairwiseSum
      result.ubig_value.push_back(pairwiseDiff);
   }

   if (leftVecSize > rightVecSize) {
      // Loop through the rest of the digits remaining in 
      // this ubig_value
      for (int i = rightVecSize; i < leftVecSize; i++) {
         int largerVecDigit = ubig_value[i];
         // Check if the digit from the previous loop produced 
         // a carryover
         if (carryover == 1) {
            largerVecDigit--;
            carryover = 0;
         }
         // Push back the remaining digits in largerVec
         result.ubig_value.push_back(largerVecDigit);
      }      
   }

   while (result.ubig_value.size() > 0 &&
    result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }
   return result;
}

ubigint ubigint::operator* (const ubigint& that) const {
   // Get the sizes of the leftVec and rightVec
   int leftVecSize = ubig_value.size();
   int rightVecSize = that.ubig_value.size();
   // Store the carryover value from intermediate multiplication between
   // 2 digits
   int carryover = 0; 
   int pairwiseDigit = 0; // Store the pairwise product of 2 digits
   
   ubigint result; // Initailize new ubigint object to store the product
   // Resize the vector to be able to hold the entire product of the 
   // leftVec and rightVec
   result.ubig_value.resize(leftVecSize + rightVecSize);

   // Multiplication algorithm
   for (int i = 0; i < leftVecSize; i++) {
      carryover = 0; 
      for (int j = 0; j < rightVecSize; j++) {
         // Perform the pairwise multiplication with carryover
         // (if defined)
         pairwiseDigit = result.ubig_value[i + j] + ubig_value[i] * 
         that.ubig_value[j] + carryover; 
         // Get the last digit of the pairwise digit multiplication
         result.ubig_value[i + j] = pairwiseDigit % 10; 
         // Use integer division to get the carryover value from the
         // pairwise digit multiplication
         carryover = pairwiseDigit / 10;
      }
      // Set the next product result to the carryover value
      result.ubig_value[i + rightVecSize] = carryover;
   }

   // Trim high order zeros
   while (result.ubig_value.size() > 0 &&
      result.ubig_value.back() == 0) {
      result.ubig_value.pop_back();
   }

   return result;
}

void ubigint::multiply_by_2() {
   // Store the carry value from the previous pairwise product
   int carryover = 0;
   // Store the product of the two digits
   int pairwiseDigitProduct = 0;
   // Get the length of the vector
   int vecLength = ubig_value.size();

   // Loop through the vector and multiply each digit by 2
   // (remainder 10), carrying over to the next digit
   for(int i = 0; i < vecLength; i++) {
      // Multiply each digit by 2 and add previous carryover
      pairwiseDigitProduct = ubig_value[i] * 2 + carryover;

      // Check if carryover required
      carryover = (pairwiseDigitProduct >= 10) ? (1) : (0);

      // Save product in proper index
      ubig_value[i] = pairwiseDigitProduct % 10;
   }

   // Check if the carryover value is 1 after the loop has finished,
   // will need to push_back 1 to the end to complete the multiplication
   // process
   if (carryover == 1) {
      ubig_value.push_back(1);
   }

   // Trim high order zeros
   while (ubig_value.size() > 0 && ubig_value.back() == 0) {
      ubig_value.pop_back();
   }
}

void ubigint::divide_by_2() {
   // Store the integer division of the two digits
   int pairwiseDigitQuotient = 0;
   // Get the length of the vector
   int vecLength = ubig_value.size();

   // Loop through the vector starting from the low order digit
   for (int i = 0; i < vecLength; i++) {
      // Divide the digit by 2
      pairwiseDigitQuotient = ubig_value[i] / 2;

      // Check if the next higher digit is odd. If it is, add 5 to the
      // current digit
      if (i < vecLength - 1) {
         if (ubig_value[i + 1] % 2 == 1) {
            pairwiseDigitQuotient += 5;
         }         
      }
      // Update the ith digit in ubig_value
      ubig_value[i] = pairwiseDigitQuotient;
   }

   // Trim high order zeros
   while (ubig_value.size() > 0 && ubig_value.back() == 0) {
      ubig_value.pop_back();
   }
}

struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   // DEBUGF ('/', "quotient = " << quotient);
   // DEBUGF ('/', "remainder = " << remainder);
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   // Validate that the size of the vectors are the same
   if (ubig_value.size() != that.ubig_value.size()) {
      return false;
   }

   // At this point, both vectors have the same size
   int vecSize = ubig_value.size(); // get the size of one vector
   // Loop through both vectors and compare each digit from low order
   for (int i = 0; i < vecSize; i++) {
      // If one value does not match with the other, the vectors are not
      // equal
      if (ubig_value[i] != that.ubig_value[i]) {
         return false;
      }
   }

   // Each digit in both vectors are the same, so the vectors are equal
   return true;
}

bool ubigint::operator< (const ubigint& that) const {
   // Store the sizes of both vectors
   int leftVecSize = ubig_value.size();
   int rightVecSize = that.ubig_value.size();

   // Validate that the size of the vectors are the same
   if (leftVecSize < rightVecSize) {
      // The left vec has more digits
      return true;
   } else if (leftVecSize > rightVecSize) { 
      // The right vec has more digits
      return false;
   }

   // Both vectors have the same size, so compare each digits starting
   // from the high order end to the low order end
   for (int i = 0; i < leftVecSize; i++) {
      // If any digit in the leftVec is less than the rightVec, then
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

   // At this point, no differences were found between the two vectors
   return false;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   // Initialize digit and num to store the converted digit
   int digitInt = 0;
   // Initialize counter to determine if character count for a line
   // has been exceeded
   int j = 0;

   // Get size of vector
   int vecSize = that.ubig_value.size();
   
   // If size == 0, print 0
   if (vecSize == 0) {
      out << 0;
      return out;
   }

   // Loop through vector and build up out from the lowend
   for (int i = vecSize - 1; i >= 0; i--) {
      // Cast the digit to an int, then to a string
      digitInt = static_cast<int>(that.ubig_value[i]);
      // Write digit to ostream
      out << digitInt;
      // Increment character count
      j++;
      // Move to the next line if the character count for the line has
      // been exceeded     
      if (j % 69 == 0 && j != 0) {
         out << "\\";
         out << "\n";
      }
   }

   return out;
}

const ubigint::ubigvalue_t& ubigint::getUBigValue() const {
   return ubig_value;
}
