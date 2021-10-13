// $Id: util.cpp,v 1.3 2021-10-07 21:14:11-07 - - $

#include <cstring>
using namespace std;

#include "util.h"

string exec::execname_; // Must be initialized from main().
int exec::status_ = EXIT_SUCCESS;

void exec::execname (const string& argv0) {
   execname_ = basename (argv0.c_str());
   cout << boolalpha;
   cerr << boolalpha;
   DEBUGF ('Y', "execname = " << execname_);
}

void exec::status (int new_status) {
   new_status &= 0xFF;
   if (status_ < new_status) status_ = new_status;
}

const string octal (long number) {
   ostringstream stream; 
   stream << showbase << oct << number;
   return stream.str();
}    

ostream& note() {
   return cerr << exec::execname() << ": ";
}

ostream& error() {
   exec::status (EXIT_FAILURE);
   return note();
}

