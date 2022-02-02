// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $
// Cody Yiu (coyiu@ucsc.edu)
// Andrew Lim (ansclim@ucsc.edu)

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <regex>
#include <cassert>
#include <vector>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

const string cin_name = "-";

/**
 * Listen for debug flags
 */
void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

/**
 * Display files given on the command line. If no files are specified,
 * display stdin. Interpret "-" as the name of stdin
 * Input: istream infile, string filename
 * Output: none
 */
void catfile (istream& infile, const string& filename, 
   str_str_map& listmap) {
   /**
    * Regular expressions for file/cin parsing. Possible input lines:
    * 1. comment_regex: #
    * 2. key_value_regex: "key = ", "key = value", "= value", "="
    * 3. trimmed_regex: "key"
    */
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};

   // Loop through file/cin
   int count = 1;
   str_str_map::iterator itr;

   for(;;) {
      // Initialize vars to store the content of each file/cin line
      // and the result of regex_search with a particular regex literal
      string line{};
      smatch result{};

      // Get next line in file/cin
      getline (infile, line);
      if (infile.eof()) {
         break;
      }

      // Determine input type using regex literals defined above
      cout << filename << ": " << count << ": ";
      cout << line << endl;
      if (regex_search(line, result, comment_regex)) {
         // Input: # or empty line
         // Output: None
      } else if (regex_search(line, result, key_value_regex)) {
         if (result[1] == "" && result[2] == "") {
            // Input: "="
            // Output: print full listmap in lexicographic order
            listmap.printMap();
         } else if (result[1] == "" && result[2] != "") {
            // Input: "= value"
            // Output: all pairs with given value in lexicographic
            // order sorted by key

            // Iterates through listmap and prints out any pair with
            // value = result[2]
            for (itr = listmap.begin(); itr != listmap.end(); ++itr) {
               if ((*itr).second == result[2]){
                  cout << (*itr).first << " = " 
                     << (*itr).second << endl;
               }
            }

         } else if (result[1] != "" && result[2] == "") {
            // Input: "key ="
            // Output: delete pair with given key
            itr = listmap.find(result[1]);
            itr = listmap.erase(itr);
         } else {
            // Input: "key = value"
            // Output: insert key and value pair into listmap
            str_str_pair newPair(result[1], result[2]);
            listmap.insert(newPair);
            cout << result[1] << " = " << result[2] << endl;
         }
      } else if (regex_search(line, result, trimmed_regex)) {
         // Input: "key"
         // Output: all pairs with given key
         // If key not found: <key>: key not found

         // Search for given key and print appropriate output
         itr = listmap.find(result[1]);
         if (itr == listmap.end()){
            cout << result[1] << ": key not found" << endl;
         } else {
            cout << (*itr).first << " = " << (*itr).second << endl;
         }
      } else {
         assert (false and "This can not happen.");
      }
      count++;
   }
}

int main (int argc, char** argv) {
   /**
    * argc = number of command line arguments passed
    * argv[0] = name of the program
    * argv[argc - 1] = command line arguments
    */
   sys_info::execname (argv[0]);
   scan_options (argc, argv);
   // Initailize listmap object
   str_str_map testMap;
   
   // optind is the integer where non-option arguments starts
   vector<string> filenames(&argv[optind], &argv[argc]);
   // Check if any filenames were passed to ./keyvalue.
   if (filenames.size() == 0) {
      // Push back cin_name; input of program will come from cin_name
      filenames.push_back(cin_name);
   }
   
   // Loop through filenames (or cin if none were provided)
   for (const auto& filename: filenames) {
      // Check if standard input was passed into filenames
      if (filename == cin_name) {
         catfile(cin, filename, testMap);
      } // Otherwise, process file argument
      else {
         ifstream infile(filename);
         // Validate that the filename is a valid file
         if (infile.fail()) {
            // Call syscall_error on the filename, which will write to
            // strerror
            syscall_error(filename);
         } else {
            catfile(infile, filename, testMap);
            infile.close();
         }
      }
   }

   // return status;
   return sys_info::exit_status();
}

