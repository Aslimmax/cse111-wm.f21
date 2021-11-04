// $Id: simple-template.cpp,v 1.7 2021-11-04 09:10:45-07 - - $

// Simple examples of a use of template functions.

#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <string>
#include <vector>
using namespace std;

// Print a pair.
template <typename type1,typename type2>
ostream& operator<< (ostream& out, const pair<type1,type2>& item) {
 return  out << "{" << item.first << "," <<item.second << "}";
}

// Print an object of arbitrary type,
// assuming operator<< exists for that object.
template <typename type>
void print (const type& v) {
   for (const auto& i: v) cout << " " << i;
   cout << endl;
}

// Print containers of variousl things.
int main() {
   print<vector<string>> ({"foo", "bar"});
   print<vector<int>> ({1,3});
   print (list<int> ({1,2,3}));

   cout << setprecision (numeric_limits<long double>::digits10);
   print<vector<double>> ({cos (-1.0L), exp (1.0L)});

   cout << boolalpha;
   print<vector<bool>> ({false,true});

   vector<string> some_vector {"hello", "world"};
   print (some_vector);

   print (map<string,int> ({{"abc",3},{"def",4}}));
}

