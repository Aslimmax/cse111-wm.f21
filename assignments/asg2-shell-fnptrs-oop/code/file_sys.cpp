// $Id: file_sys.cpp,v 1.15 2021-10-29 13:30:54-07 - - $

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

/**
 * Get the current promopt
 * Input: None
 * Output: string prompt
 */
const string& inode_state::prompt() const { return prompt_; }

/**
 * Change the prompt
 * Input: string of the new promopt
 * Output: none
 */
void inode_state::prompt(const string &newPrompt) {
   prompt_ = newPrompt;
}

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
           fileType = type;
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           fileType = type;
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

/* Get file_type of contents 
Input: None
Output: file_type
 */
file_type inode::getFileType() const {
   return fileType;
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

wordvec base_file::getData() const {
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


/* Get the size of plain_file wordvec data
Input: None
Output: number of characters in data
 */
size_t plain_file::size() const {
   // Initailize size
   size_t size = 0;

   // Loop through all elements in data
   for (wordvec::const_iterator iter = data.begin(); iter != data.end();
      ++iter) {
      // Increment size based on the length of the string in wordvec
      size += (*iter).length();
   }
   
   DEBUGF ('i', "size = " << size);
   return size;
}

/* Return data
Input: None
Output: wordvec data (class member of plain_file) 
 */
const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

/* Replace the content of data
Input: wordvec to replace current data
Output: None
 */
void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
}

wordvec plain_file::getData() const {
   return data;
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

   // Check if filename exists in dirents
   map<string, inode_ptr>::iterator iter = dirents.find(filename);
   if (iter == dirents.end()) { // filename has not been found
      return;
   }

   // Check if directory is empty
   if (iter->second->getContents()->getDirents().size() == 2) {
      // iter->second->setCwd(nullptr);
      dirents.erase(iter);
   }
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   
   // Check if a directory or file called dirname already exists
   map<string, inode_ptr>::iterator iter = dirents.find(dirname);
   if (iter != dirents.end()) {
      return nullptr;
   }

   /* TODO:
   Add check for 'the complete pathname of the parent of this new
   directory does not already exist' */
   // Make a new inode_state obj
   // inode_state directoryToAdd;

   // Get the inode ptr
   // inode_ptr tempPtr = directoryToAdd.getCwd();
   inode_ptr inodePtr = make_shared<inode>(file_type::DIRECTORY_TYPE);

   // Add new directory to dirents
   addDirectoryContent(dirname, inodePtr);

   // Add . our new directory
   inodePtr->getContents()->addDirectoryContent(".", inodePtr);

   // Get the inode_ptr of the root
   iter = dirents.find("..");
   // Add .. to our new directory
   inodePtr->getContents()
      ->addDirectoryContent("..", iter->second);

   return inodePtr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   
   // Check if a directory or file called filename already exists
   map<string, inode_ptr>::iterator iter = dirents.find(filename);
   if (iter != dirents.end()) { // Found the element
      // Ensure that the element is not a directory
      return iter->second;
   }
   
   // Make a new file obj to add to dirents
   // inode newFile(file_type::PLAIN_TYPE);
   inode_ptr filePtr = make_shared<inode>(file_type::PLAIN_TYPE);

   addDirectoryContent(filename, filePtr);

   return filePtr;
}

