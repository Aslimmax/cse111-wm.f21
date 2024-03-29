// $Id: file_sys.cpp,v 1.24 2021-10-31 02:22:12-07 - - $

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
 * Get the current prompt
 * Input: None
 * Output: string prompt
 */
const string& inode_state::prompt() const { return prompt_; }

/**
 * Change the prompt
 * Input: string of the new promopt
 * Output: none
 */
void inode_state::setPrompt(const string &newPrompt) {
   prompt_ = newPrompt;
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

/**
 * Get root of current state
 * Input: None
 * Output: inode_ptr to class member root
 */
inode_ptr inode_state::getRoot() const {
   return root;
}

/**
 * Get cwd of current state
 * Input: None
 * Output: inode_ptr to class member cwd
 */
inode_ptr inode_state::getCwd() const {
   return cwd;
}

/**
 * Get filepath of inode_state
 * Input: None
 * Output: string of the current path
 */
string inode_state::getFilepath() {
   string finalPathOutput = ""; // initiailize output path

   // Check if filepath is currently at the root
   if (filepath.empty()) {
      finalPathOutput += "/";
   } else {
      // Loop through filePath iterator
      for (vector<string>::const_iterator iter = filepath.begin(); 
         iter != filepath.end(); ++iter) {
         finalPathOutput += "/" + (*iter);
      }      
   }

   return finalPathOutput;
}

/**
 * Set the cwd
 * Input: inode_ptr to pointer to
 * Output: none
 */
void inode_state::setCwd(const inode_ptr& newPtr) {
   cwd = newPtr;
}

/**
 * Set filepath
 * Input: string new filepath
 * Output: none
 */
void inode_state::setFilepath(const string& newFilepath) {
   // Process the filepath string
   wordvec path = split(newFilepath, "/");

   // Copy words into filepath
   filepath = path;
}

/**
 * Push back a new path to filepath
 * Input: string new path to add
 * Output: none
 */
void inode_state::pushFilepath(const string& newFilepath) {
   filepath.push_back(newFilepath);
}

/**
 * Pop back the path at the end of filepath
 * Input: none
 * Output: none
 */
void inode_state::popFilepath() {
   filepath.pop_back();
}

/**
 * Clear the filepath vector
 * Input: None
 * Output: None
 */
void inode_state::resetFilePath() {
   filepath.clear();
}

/**
 * Overloaded inode constructor 
 * Input: file type
 * Output: None
 */
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

/**
 * Get inode number
 * Input: None
 * Output: size_t unique inode number
 */
size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

/**
 * Get base_file_ptr contents
 * Input: None
 * Output: base_file_ptr contents member
 */
base_file_ptr inode::getContents() const {
   return contents;
}

/**
 * Set base_file_ptr contents
 * Input: base_file_ptr new contents
 * Output: none
 */
void inode::setContents(base_file_ptr newContents) {
   contents = newContents;
}

/**
 * Get file_type of contents
 * Input: None
 * Output: file_type
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

/**
 * Get the size of plain_file wordvec data
 * Input: none
 * Output: output of characters in data
 */
size_t plain_file::size() const {
   // Initailize size
   size_t size = 0;
   size_t numWords = 0; // initialze number of words counter

   // Loop through all elements in data
   for (wordvec::const_iterator iter = data.begin(); iter != data.end();
      ++iter) {
      // Increment size based on the length of the string in wordvec
      size += (*iter).length();
      numWords++;
   }

   // Calculate the number of characters in the file
   if (numWords != 0) {
      size = size + numWords - 1;

   }

   DEBUGF ('i', "size = " << size);
   return size;
}

/**
 * Return data
 * Input: none
 * Output: wordvec data
 */
const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

/**
 * Replace the content of data
 * Input: wordvec to replace current data
 * Output: none
 */
void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
}

/**
 * Get data member of plain_file object
 * Input: none
 * Output: wordvec data
 */
wordvec plain_file::getData() const {
   return data;
}

/**
 * Add new file/directory to dirents
 * Input: string file/directory name, the cwd of that file/directory
 * Output: none
 */
void directory::addDirectoryContent(const string& filename, 
   const inode_ptr& inodePtr) {
   // Create a tempPair obj to insert into dirents
   pair<string, inode_ptr> tempPair(filename, inodePtr);
   // Insert into dirents
   dirents.insert(tempPair);
}

/**
 * Get dirents member from derviced class directory
 * Input: None
 * Output: map<string, inode_ptr> dirents member
 */
map<string, inode_ptr> directory::getDirents() const {
   return dirents;
}

/**
 * Get the number of entries in the directory
 * Input: None
 * Output: size_t number of entires in directory
 */
size_t directory::size() const {
   size_t size = dirents.size();

   DEBUGF ('i', "size = " << size);
   // return size;
   return size;
}

/**
 * Remove the file/directory from dirents
 * Input: string name of the file/directory to be removed
 * Output: none
 */
void directory::remove (const string& filename) {
   DEBUGF ('i', filename);

   // Find the file
   map<string, inode_ptr>::iterator iter = dirents.find(filename);
   
   // Check if the file is a directory
   if (iter->second->getFileType() == file_type::DIRECTORY_TYPE) {
      // Look inside of the directory
      map<string, inode_ptr> tempDirents = 
         iter->second->getContents()->getDirents();      
      // Validate that the directory is empty
      if (tempDirents.size() != 2) {
         throw file_error(filename + ": cannot be removed, not empty");
      }

      // Set root and cwd to null
      tempDirents.find(".")->second->setContents(nullptr);
      tempDirents.find(".")->second = nullptr;
      tempDirents.erase(".");
      tempDirents.erase("..");
   }
   // Erase the file/directory
   dirents.erase(iter);   
}

/**
 * Create a new directory in dirents
 * Input: string name of the directory
 * Output: inode_ptr of the directory
 */
inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   
   map<string, inode_ptr>::iterator iter;

   // Get the inode ptr
   // inode_ptr tempPtr = directoryToAdd.getCwd();
   inode_ptr inodePtr = make_shared<inode>(file_type::DIRECTORY_TYPE);

   // Add new directory to dirents
   addDirectoryContent(dirname, inodePtr);

   // Add . our new directory
   inodePtr->getContents()->addDirectoryContent(".", inodePtr);

   // Get the inode_ptr of the cwd
   iter = dirents.find(".");
   // Add .. to our new directory
   inodePtr->getContents()
      ->addDirectoryContent("..", iter->second);

   return inodePtr;
}

/**
 * Create a new file in dirents
 * Input: string name of the file
 * Output: inode_ptr of the directory
 */
inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   
   // Check if a directory or file called filename already exists
   map<string, inode_ptr>::iterator iter = dirents.find(filename);
   if (iter != dirents.end()) { // Found the element
      // Ensure that the element is not a directory
      if (iter->second->getFileType() == file_type::DIRECTORY_TYPE) {
         return nullptr;
      }
      return iter->second;
   }
   
   // Make a new file obj to add to dirents
   // inode newFile(file_type::PLAIN_TYPE);
   inode_ptr filePtr = make_shared<inode>(file_type::PLAIN_TYPE);

   // Add the file to dirents
   addDirectoryContent(filename, filePtr);

   return filePtr;
}

