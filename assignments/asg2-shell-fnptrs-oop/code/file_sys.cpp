// $Id: file_sys.cpp,v 1.11 2021-10-27 15:19:03-07 - - $

#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

ostream& operator<< (ostream& out, file_type type) {
   switch (type) {
      case file_type::PLAIN_TYPE: out << "PLAIN_TYPE"; break;
      case file_type::DIRECTORY_TYPE: out << "DIRECTORY_TYPE"; break;
      default: assert (false);
   };
   return out;
}

inode_state::inode_state() {
   // Make an inode pointer
   inode_ptr directoryPtr = 
      make_shared<inode>(file_type::DIRECTORY_TYPE);

   // Set root and CWD to the new pointer (on first init)
   root = directoryPtr;
   cwd = root;

   // Set the correct string paths for root and cwd
   root->contents->addDirectoryContent("..", root);
   root->contents->addDirectoryContent(".", cwd);

   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

/* Get root of inode_state 
Input: None
Output: inode_ptr to class member root
*/
inode_ptr inode_state::getRoot() const {
   return root;
}

/* Get cwd of inode_state 
Input: None
Output: inode_ptr to class member cwd
*/
inode_ptr inode_state::getCwd() const {
   return cwd;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
      default: assert (false);
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

/* Get inode number
Input: None
Output: size_t inode number
 */
size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

/* Get base_file_ptr contents
Input: None
output: base_file_ptr contents member
 */
base_file_ptr inode::getContents() const {
   return contents;
}

file_error::file_error (const string& what):
            runtime_error (what) {
}

void base_file::addDirectoryContent(const string&, const inode_ptr&) {
   throw file_error ("is a " + error_file_type());
}

map<string, inode_ptr> base_file::getDirents() const {
   throw file_error ("is a " + error_file_type());
}

const wordvec &base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

void directory::addDirectoryContent(const string& filename, 
   const inode_ptr& inodePtr) {
   // Create a tempPair obj to insert into dirents
   pair<string, inode_ptr> tempPair(filename, inodePtr);
   // Insert into dirents
   dirents.insert(tempPair);
}

/* Get dirents member from derived class directory
Input: None
Output: map<string, inode_ptr> dirents member
 */
map<string, inode_ptr> directory::getDirents() const {
   return dirents;
}

/* Get the number of entries in the directory
Input: None
Output: size_t number of entries in directory
 */
size_t directory::size() const {
   size_t size = 0; // initialize size to 0
   for (map<string, inode_ptr>::iterator iter = getDirents().begin();
         iter != getDirents().end(); ++iter) {
      size++; // increment the size
   }
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);

   // Make a new inode_state obj
   inode_state directoryToAdd;

   // Set the cwd of directoryToAdd to dirname
   directoryToAdd

   // initialize string to hold file path of cwd
   string cwdFilePath = ""; 
   
   // Find the current path of the current directory
   // for (map<string, inode_ptr>::iterator iter = dirents.begin(); 
   //    iter != dirents.end(); ++iter) {
   //    if (iter->first == ".") { // found the cwd

   //    }
   // }

   // Set the correct string paths for root and cwd
   return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}

