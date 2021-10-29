// $Id: commands.cpp,v 1.26 2021-10-29 12:45:52-07 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};
/*
 * Return the command in cmd_hash unordered map based on command passed
 * 
 * Input: constant string reference cmd
 * Output: command_fn to which function needs to be executed. If the
 * command doesn't exist, throw a command_error
 */
command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result {cmd_hash.find (cmd)};
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such command");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status {exec::status()};
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

/* Copy the contents of each file to the standard output
Input: wordvec words
Output: None, but the contents of words is printed to the standard
output
 */
void fn_cat (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Check that a file is specified
   if (words.size() < 1) {
      throw command_error(words.at(0) + ": no file specified");
   }

   // Look for the specified file in dirents
   map<string, inode_ptr> tempDirents = 
      state.getCwd()->getContents()->getDirents();
   map<string, inode_ptr>::iterator iter =
      tempDirents.find(words.at(1));

   if (iter == tempDirents.end()) { // file wasn't found
      throw command_error(words.at(1) + ": file does not exist");
   }

   // If the file was found, check that it is not a directory
   inode_ptr inodePtr = iter->second;
   bool isDirectory = (inodePtr->getFileType() ==
      file_type::DIRECTORY_TYPE) ? true : false;

   if (isDirectory) {
      throw command_error(words.at(1) + ": this is a directory");
   }

   // File found is a plain_file class
   string outputString = ""; // initialize output string of data
   wordvec fileData = dynamic_pointer_cast<plain_file>
      (inodePtr->getContents())->getData();
   // Loop through plain_file data
   for (wordvec::const_iterator wordIter = fileData.begin(); 
      wordIter != fileData.end(); ++wordIter) {
      outputString += (*wordIter) + " "; // build output string
   }
   // Remove the extra white space at the end of the string
   outputString.pop_back();

   cout << outputString << endl;
}

void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

/* Echos words, which may be empty, to the standard output on a line
by itself
Input: inode_state& obj, wordvec& string vector
Output: None
 */
void fn_echo (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // If no arguments are passed with the command, then dot is used as
   // its operand
   if (words.size() < 1) {
      cout << "/:" << endl;
   } else {
      cout << "/:" << endl;
   }

   // Set a temporary var to class member dirents
   map<string, inode_ptr> tempDirEnts = 
      state.getCwd()->getContents()->getDirents();
   // Loop through dirents
   for (map<string, inode_ptr>::iterator iter = tempDirEnts.begin();
        iter != tempDirEnts.end(); ++iter)
   {
      // Stores whether or not the element is a file or directory
      bool isDirectory = true;
      size_t inodeNum = iter->second->get_inode_nr();
      string filePath = iter->first;
      int fileSize = 0;
      inode_ptr inodePtr = iter->second;
            
      // Determine file type
      isDirectory = (inodePtr->getFileType() == 
         file_type::DIRECTORY_TYPE) ? true : false; 
      
      if (isDirectory) {
         fileSize = inodePtr->getContents()->getDirents().size();
         filePath += "/";
      } 
      else { // Otherwise, the file is a PLAIN_TYPE
         fileSize = dynamic_pointer_cast<plain_file>
            (inodePtr->getContents())->size();
      }

      // Print out the completed line
      cout << setw(6) << inodeNum << setw(6) << fileSize << "  "
         << filePath << endl;
   }
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Check to see if no arguments were provided
   if (words.size() <= 1) {
      throw command_error(words[0] + ": missing file name");
   }

   // Make and insert an empty file into dirents
   inode_ptr filePtr = state.getCwd()->getContents()->mkfile(words[1]);

   // Check if words were passed to the argument
   // words[0] = command name
   // words[1] = file name
   if (words.size() > 2) {
      // Copy the words into fileContents
      wordvec fileContents = wordvec(words.begin() + 2, words.end());
      // Write to the file
      dynamic_pointer_cast<plain_file>
         (filePtr->getContents())->writefile(fileContents);
   }
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   
   // Ensure that a duplicate folder will not be added
   if (state.getCwd()->getContents()->mkdir(words[1]) == nullptr) {
      throw command_error(words[1] + ": directory already exists");
   }
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Check if no arguemnts were provided
   if (words.size() <= 1) {
      throw command_error(words[0] + ": missing prompt argument");
   }

   string newPromptOutput = ""; // initialize final output
   // Get the new prompt provided by the user
   wordvec newPrompt = wordvec(words.begin() + 1, words.end());

   // Loop through all words in newPrompt
   for (wordvec::iterator iter = newPrompt.begin(); 
      iter != newPrompt.end(); ++iter) {
      // Append each word to the final output
      newPromptOutput += (*iter) + " "; 
   }

   // Change the prompt
   state.prompt(newPromptOutput);
}

/* Prints the current working directory
Input: inode_state obj, words ([0] => name of command, [1] => arguments)
Output: void function
 */
void fn_pwd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);   
   // initialize output string for the final path
   string outputPath = ""; 
   // If cwd points to root, then we are at the root directory
   if (state.getRoot()->get_inode_nr() == 1) {
      outputPath += "/";
   }
   else {

   }
   
   cout << outputPath << endl;
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   state.getCwd()->getContents()->remove(words[1]);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

