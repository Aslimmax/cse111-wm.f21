// $Id: commands.cpp,v 1.29 2021-10-29 17:35:41-07 - - $

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

/**
 * Copy the contents of each file to the standard output
 * Input: inode_state obj, wordvec string vector
 * Output: none
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
   // Get the fileData
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

/**
 * Set the current directory to the pathname given. If no pathname is
 * specified, the root direcotry (/) is used
 * Input: inode_state obj, wordvec string vector
 * Output: none
 */
void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Check that a directory is specified (if not, use the root
   // directory)
   if (words.size() == 1) {
      state.setCwd(state.getRoot());
      // Reset the filepath
      state.resetFilePath();
      return;
   } 

   // Check if more than one operand is given
   if (words.size() > 2) {
      throw command_error(words.at(0) + ": too many operands given");
   }

   // Process the path
   wordvec path = split(words.at(1), "/");

   // Get the ptr to the directory specified 
   inode_ptr finalCwdPtr = validPath(state, words);

   // At this point, we have found the directory and confirmed that it 
   // is a directory. Can now update the cwd and filepath

   // Check if there are multiple paths specified
   if (path.size() == 1) {
      // Exit function is trying to cd into itself
      if (path.at(0) == ".") {
         return;
      } // Pop back last element in filepath if going to parent dir
      else if (path.at(0) == "..") {
         state.popFilepath();
      } else { // Otherwise, udpate the filepath
         state.pushFilepath(path.at(0));
      }
   } else {
      // Loop through path and push back to filepath to cwd
      for (wordvec::iterator pathIter = path.begin();
         pathIter != path.end(); ++pathIter) {
         state.pushFilepath((*pathIter));
      }
   }

   // Update cwd
   state.setCwd(finalCwdPtr);
}

/* Echos words, which may be empty, to the standard output on a line
by itself
Input: inode_state obj, wordvec string vector
Output: None
 */
void fn_echo (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}

/**
 * Exit the shell (STILL NEED TO CONFIGURE)
 * Input: inode_state obj, wordvec string vector
 * Output: none
 */
void fn_exit (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

/**
 * Print out a description of the files or directories to the standard
 * output. If no pathname is specified, the current working directory is
 * used.
 * Input: inode_state obj, wordvec string vector
 * Output: none
 */
void fn_ls (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // If no arguments are passed with the command, then dot is used as
   // its operand
   // Print out the pathname specified
   cout << state.getFilepath() + ":" << endl;

   // Check to see if the path supplied is valid
   inode_ptr directoryPath = validPath(state, words);

   // Set a temporary var to class member dirents
   map<string, inode_ptr> tempDirents = 
      directoryPath->getContents()->getDirents();
   // Loop through dirents
   for (map<string, inode_ptr>::iterator iter = tempDirents.begin();
        iter != tempDirents.end(); ++iter)
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

   // Check to see if no arguments were provided
   if (words.size() < 2) {
      throw command_error(words[0] + ": missing directory name");
   }
   
   // Ensure that a duplicate folder will not be added
   if (state.getCwd()->getContents()->mkdir(words[1]) == nullptr) {
      throw command_error(words[1] + ": directory already exists");
   }
}

/**
 * Update the current terminal prompt
 * Input: inode_state obj, wordvec string vector
 * Output: none
 */
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
   state.setPrompt(newPromptOutput);
}

/**
 * Prints the current working directory
 * Input: inode_state obj, wordvec string vector
 * Output: none
 */
void fn_pwd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);   
   // initialize output string for the final path
   string outputPath = ""; 

   outputPath = state.getFilepath();
   
   cout << outputPath << endl;
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Check if no arguemnts were provided
   if (words.size() <= 1) {
      throw command_error(words[0] + ": missing operand");
   }

   state.getCwd()->getContents()->remove(words[1]);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

/**
 * Check to see if the pathname given leads to a valid directory
 */
inode_ptr validPath(inode_state& state, const wordvec& words) {
   // Process the path
   wordvec path = split(words.at(1), "/");
   // Initialize pointer to the cwd
   inode_ptr finalCwdPtr = state.getCwd();

   // Loop through each path, throw an error if a path doesn't exist
   for (wordvec::iterator wordIter = path.begin();
        wordIter != path.end(); ++wordIter)
   {
      // Get the dirents of the cwd
      map<string, inode_ptr> tempDirents =
          finalCwdPtr->getContents()->getDirents();

      // Look for the element based on the given path
      map<string, inode_ptr>::iterator dirIter =
          tempDirents.find((*wordIter));

      if (dirIter == tempDirents.end())
      { // file wasn't found
         throw command_error(words.at(1) +
                             ": directory does not exist");
      }

      // If the directory was found, check that it is not a file
      inode_ptr dirPtr = dirIter->second;
      bool isFile = (dirPtr->getFileType() == file_type::PLAIN_TYPE)
         ? true : false;
      
      // Throw an error if dirPtr is a file
      if (isFile) { 
         throw command_error(words.at(1) + ": Not a directory");
      }

      // Update finalCwdPtr and continue to loop through path
      finalCwdPtr = dirPtr;
   }

   return finalCwdPtr;
}

