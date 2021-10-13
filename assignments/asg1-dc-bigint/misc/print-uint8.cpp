// $Id: print-uint8.cpp,v 1.21 2021-04-12 16:07:23-07 - - $

// Show how to print a uint8_t as a number.
// Options:
// -c as characters
// -n as numbers

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
using namespace std;

#ifdef __GNUG__

   #include <cxxabi.h>
   string demangle (const char* name) {
      int status = 0;
      unique_ptr<char,void(*)(void*)> result {
         abi::__cxa_demangle (name, nullptr, nullptr, &status),
         std::free,
      };
      return status == 0 ? result.get() : name; 
   }

#else

   string demangle (const char* name) {
      return name;
   }

#endif

string basename (const string &name) {
   return name.substr (name.rfind ('/') + 1);
}

template <typename T>
void print() {
   const char* type = typeid(T).name();
   cout << "Printing as " << type << " = " << demangle (type) << endl;
   cout << "[";
   for (uint8_t n = 0; n <= 9; ++n) cout << T(n);
   cout << "]" << endl;
}

unordered_map<string,void(*)()> fns {
   {"-8", print<uint8_t>},
   {"-i", print<int>},
   {"-u", print<unsigned>},
};

int main (int argc, char** argv) {
   string opt = argc == 1 ? "" : argv[1];
   auto fn = fns.find (opt);
   if (fn == fns.end()) print<uint8_t>();
                   else fn->second();
}

