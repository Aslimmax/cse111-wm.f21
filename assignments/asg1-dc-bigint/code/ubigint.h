// $Id: ubigint.h,v 1.7 2021-10-14 22:18:21-07 - - $

#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>
#include <cstdint>
using namespace std;

#include "debug.h"

class ubigint {
   friend ostream& operator<< (ostream&, const ubigint&);
   private:
        using ubigvalue_t = vector<uint8_t>;
        ubigvalue_t ubig_value;
   public:
        void multiply_by_2();
        void divide_by_2();
 
        ubigint() = default; // Need default ctor as well.
        ubigint (unsigned long);
        ubigint (const string&);

        ubigint operator+ (const ubigint&) const;
        ubigint operator- (const ubigint&) const;
        ubigint operator* (const ubigint&) const;
        ubigint operator/ (const ubigint&) const;
        ubigint operator% (const ubigint&) const;

        bool operator== (const ubigint&) const;
        bool operator<  (const ubigint&) const;

        // Helper methods
        // Returns the private member ubig_value
        const ubigvalue_t& getUBigValue() const;
};

#endif

