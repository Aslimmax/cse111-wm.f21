// $Id: commands.cpp,v 1.33 2021-10-30 14:21:03-07 - - $

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

   // Check that a pathname is specified
   if (words.size() < 1) {
      throw command_error(words.at(0) + ": no file specified");
   }

   // Determine the number of pathnames supplied
   wordvec pathnames = wordvec(words.begin() + 1, words.end());

   inode_ptr directoryPath; // initailize directoryPath

   // Loop through all pathnames specified and copy the contents of each
   // file (if they exist) to the standard output
   for (int i = 0; i < static_cast<int>(pathnames.size()); i++) {
      // Reset directory path
      directoryPath = state.getCwd();
      // Check to see if each pathname supplied is a single element
      wordvec pathname = split(words.at(i + 1), "/");
      
      if (pathname.size() != 1) {
         pathname = wordvec(words.begin() + i, words.end());

         // Check to see if pathname is valid
         directoryPath = validPath(directoryPath, pathname);

         // Update pathname for parsing with determineFileType
         pathname = wordvec(words.begin() + i + 1, words.end());
      }

      // Determine the file type of the last path
      directoryPath = determineFileType(directoryPath, pathname);

      // If the lastpath points to an element, determine if it's a file
      // or directory
      if (directoryPath->getFileType() == file_type::DIRECTORY_TYPE) {
         throw command_error(words.at(i + 1) + ": Is a directory");
      }

      // Otherwise, element found is a plain_file class
      string outputString = ""; // initialize output string of data
      // Get the fileData
      wordvec fileData = dynamic_pointer_cast<plain_file>
         (directoryPath->getContents())->getData();
      // Loop through plain_file data
      for (wordvec::const_iterator wordIter = fileData.begin(); 
         wordIter != fileData.end(); ++wordIter) {
         outputString += (*wordIter) + " "; // build output string
      }
      // Remove the extra white space at the end of the string
      outputString.pop_back();

      cout << outputString << endl;
   }
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

   inode_ptr directoryPath = state.getCwd();

   // Check to see if each pathname supplied is a single element
   wordvec pathname = split(words.at(1), "/");
   if (pathname.size() != 1) {
      pathname = wordvec(words.begin(), words.end());

      // Check to see if pathname is valid
      directoryPath = validPath(directoryPath, pathname);

      // Update pathname for parsing with determineFileType
      pathname = wordvec(words.begin() + 1, words.end());
   }

   // Determine the file type of the last path
   directoryPath = determineFileType(directoryPath, pathname);

   // Check if the element is a valid directory
   bool isFile = (directoryPath->getFileType() == file_type::PLAIN_TYPE)
      ? true : false;

   // Throw an error if directoryPath is a file
   if (isFile) { 
      throw command_error(words.at(1) + ": Not a directory");
   }

   // At this point, we have found the directory and confirmed that it 
   // is a directory. Can now update the cwd and filepath

   // Process the path
   wordvec path = split(words.at(1), "/");
   // Loop through the valid path and update the pathname accordingly
   for (wordvec::iterator pathIter = path.begin();
      pathIter != path.end(); ++pathIter) {
      // Exit function is trying to cd into itself
      if ((*pathIter) == ".") {
         return;
      } // Pop back last element in filepath if going to parent dir
      else if ((*pathIter) == "..") {
         // Don't do anything if cwd == root
         if (state.getCwd() != state.getRoot()) {
            state.popFilepath();
         }
      } else { // Otherwise, udpate the filepath
         state.pushFilepath((*pathIter));
      }  
   }

   // Update cwd
   state.setCwd(directoryPath);
}

/**
 * Echos words, which may be empty, to the standard output on a line
 * by itself
 * Input: inode_state obj, wordvec string vector
 * Output: none
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

   // Check if too many operands were specified
   if (words.size() > 2) {
      throw command_error(words.at(0) + ": too many operands");
   }

   // Check if no arguments were specified
   if (words.size() == 1) {
      exec::status(0);
   }
   else {
      // Set the status bassed on the argument passed, but check to see 
      // if a non-numeric argument was passed
      bool isNumeric = true;
      string exitArg = words.at(1);
      int sizeOfArg = exitArg.length();

      // Determine if the input is non-numeric
      for (int i = 0; i < sizeOfArg; i++) {
         // If at least one digit is non-numeric, break the loop and set
         // isNumeric to false
         if (exitArg[0] == '-' && sizeOfArg != 1) {
            continue;
         }
         if (isdigit(exitArg[i]) == false) {
            isNumeric = false;
            break;
         }
      }

      if (!isNumeric) {
         exec::status(127);
      } else { // DEBUG LATER
         exec::status(atoi(exitArg.c_str()));
      }
   }

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

   // Initialize pointer to the cwd's directory
   inode_ptr directoryPath = state.getCwd();

   // Initialize pathname to output depending on what path is specified
   string outputPath = "";

   // Initailize a wordvec that stores the pathname specified
   wordvec pathname;

   // Check if a pathname was specified (if none was provided, use the
   // current working directory)
   if (words.size() != 1) {
      // Check to see if the pathname supplied is a single element
      pathname = split(words.at(1), "/");
      if (pathname.size() != 1)
      {
         pathname = wordvec(words.begin(), words.end());

         // Check to see if pathname is valid
         directoryPath = validPath(directoryPath, pathname);

         // Update pathname for parsing with determineFileType
         pathname = wordvec(words.begin() + 1, words.end());
      }

      // Determine the file type of the last path
      directoryPath = determineFileType(directoryPath, pathname);

      // Update the output path name (NEED TO FIX)
      outputPath += "/" + words.at(1) + ":";
   } else {
      // Update the output path name
      outputPath += state.getFilepath() + ":";
   }

   // Determine if the lastpath name specified is a file
   if (directoryPath->getFileType() == file_type::PLAIN_TYPE) {
      // Print out the name of the file and exit
      cout << pathname.back() << endl;
      return;
   }

   // Print out the pathname of the directory that will have its
   // contents printed out
   cout << outputPath << endl;

   // Print out the contents of the directory
   printDirectoryContent(directoryPath);
}

/**
 * Similar to ls, but does a recursive depth-first preorder traversal
 * for subdirectories
 */
void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

  // Initialize pointer to the cwd's directory
   inode_ptr directoryPath = state.getCwd();

   // Initialize pathname to output depending on what path is specified
   string outputPath = "";

   // Initailize a wordvec that stores the pathname specified
   wordvec pathname;

   // Check if a pathname was specified (if none was provided, use the
   // current working directory)
   if (words.size() != 1) {
      // Check to see if the pathname supplied is a single element
      pathname = split(words.at(1), "/");
      if (pathname.size() != 1)
      {
         pathname = wordvec(words.begin(), words.end());

         // Check to see if pathname is valid
         directoryPath = validPath(directoryPath, pathname);

         // Update pathname for parsing with determineFileType
         pathname = wordvec(words.begin() + 1, words.end());
      }

      // Determine the file type of the last path
      directoryPath = determineFileType(directoryPath, pathname);

      // Update the output path name (NEED TO FIX)
      outputPath += "/" + words.at(1) + ":";
   } else {
      // Update the output path name
      outputPath += state.getFilepath() + ":";
   }

   // Determine if the lastpath name specified is a file
   if (directoryPath->getFileType() == file_type::PLAIN_TYPE) {
      // Print out the name of the file and exit
      cout << pathname.back() << endl;
      return;
   }

   // Print out the pathname of the directory that will have its
   // contents printed out
   cout << outputPath << endl;

   // Print out the contents of the directory
   printDirectoryContent(directoryPath);

   /**
    * RECURSION DRIVER IMPLEMENTATION, BUCKLE UP
    */
   
   // 3. Initialize a list of all directories in the current directory
   map<string, inode_ptr> directoryList;

   // Get the dirents of the cwd
   map<string, inode_ptr> tempDirents =
       directoryPath->getContents()->getDirents();
   // Loop through dirents and look for all directories in the cwd
   for (map<string, inode_ptr>::iterator iter = tempDirents.begin();
        iter != tempDirents.end(); ++iter) {
      // Get the inodePtr of the current element that iter points to
      string directoryName = iter->first;
      inode_ptr inodePtr = iter->second;
      
      // Found a directory
      if (inodePtr->getFileType() == file_type::DIRECTORY_TYPE) {
         // Make sure they aren't "." and ".."
         if (directoryName != "." && directoryName != "..") {
            // Insert directory into directoryList
            directoryList.insert((*iter));            
         }
      }
   }

   // If the only elements in the directory are "." and "..", break
   // out of recursive call
   if (directoryList.size() == 0) {
      return;
   }

   // Loop through directoryList and make recursive calls to print
   // out more elements
   for (map<string, inode_ptr>::iterator iter = directoryList.begin();
      iter != directoryList.end(); ++iter) {
      // Initialize the next path to traverse through
      wordvec recursivePath = words;
      // Get the directory name
      string updatedPath = iter->first;

      // If words only had one argument (lsr) add the next directory
      // to traverse
      if (recursivePath.size() == 1) {
         recursivePath.push_back(updatedPath);
      } // Otherwise, append the next path to search
      else {
         recursivePath.at(1).append("/" + updatedPath);
      }

      // Make recursive call with updated path
      fn_lsr(state, recursivePath);
   }
}

/**
 * The file specified is created and the rest of the words are put into
 * that file. If the file already exists, replace its contents.
 * Input: inode_state obj, wordvec string vector
 * Output: none
 */
void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Check to see if no arguments were provided
   if (words.size() < 2) {
      throw command_error(words[0] + ": missing file name");
   }

   inode_ptr directoryPath = state.getCwd();

   // Check to see if each pathname supplied is a single element
   wordvec pathname = split(words.at(1), "/");
   if (pathname.size() != 1)
   {
      // Determine if the penultimate path is valid
      pathname = wordvec(words.begin(), words.end());

      // cout << "Before" << endl;
      directoryPath = validPath(directoryPath, pathname);
      // cout << "After" << endl;

      pathname = split(words.at(1), "/");
   }

   // Make and insert an empty file into dirents
   directoryPath = 
      dynamic_pointer_cast<directory>(directoryPath->getContents())
      ->mkfile(pathname.back());

   // Validate that if the element exists, it is not a directory
   if (directoryPath == nullptr) {
      throw command_error (words.at(1) + ": Not a file");
   }

   // Check if words were passed to the argument
   if (words.size() > 2) {
      // Copy the words into fileContents
      wordvec fileContents = wordvec(words.begin() + 2, words.end());
      // Write to the file
      dynamic_pointer_cast<plain_file>
         (directoryPath->getContents())->writefile(fileContents);
   }
}

/**
 * Create a new directory. Two entries are automatically added to this
 * directory, namely dot (.) and dotdot (..).
 * Input: inode_state obj, wordvec, string vector
 * Output: none
 */
void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Check to see if no arguments were provided
   if (words.size() < 2) {
      throw command_error(words[0] + ": missing directory name");
   }

   // Check that additional arugments were provided
   if (words.size() > 2) {
      throw command_error(words[0] + ": too many operands");
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

/**
 * Delete the specified file or directory (removed from its parent's
 * list of files and subdirectories). If the pathname is a directory,
 * it must be empty.
 * Input: inode_state obj, wordvec string vector
 * Output: none
 */
void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // Check if no arguemnts were provided
   if (words.size() < 2) {
      throw command_error(words[0] + ": missing operand");
   }

   // Initialize pointer to the cwd's directory
   inode_ptr directoryPath = state.getCwd();

   // wordvec lastPath = words.end());
   wordvec lastPath;
   lastPath.push_back(words.back());
   
   // Check if a pathname was specified
   if (words.size() > 2) {
      // Get the directory path minus the last pathname
      wordvec tempDir = wordvec(words.begin(), words.end() - 1);

      // Determine if the subpath is valid
      directoryPath = validPath(directoryPath, tempDir);
   }
   
   // Check to see if the file/directory exists
   map<string, inode_ptr> tempDirents = 
      directoryPath->getContents()->getDirents();
   map<string, inode_ptr>::iterator iter =
      tempDirents.find(lastPath.at(0));
   if (iter == tempDirents.end()) {
      throw command_error(words.at(1) + ": No such file or directory");
   }

   // Check if the file/directory is a directory
   bool isDirectory = (iter->second->getFileType() ==
      file_type::DIRECTORY_TYPE) ? true : false;

   // Check if it's a directory and if it's "empty"
   if (isDirectory) {
      // Get the size of the directory
      size_t directorySize = dynamic_pointer_cast<directory>
         (iter->second->getContents())->getDirents().size();
      if (directorySize != 2) {
         throw command_error(words.at(1) + ": directory is not empty");
      }
   }

   // Remove the file/directory
   directoryPath->getContents()->remove(lastPath.at(0));
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

/**
 * Check to see if the pathname given leads to a valid directory
 * Input: inode_state obj, wordvec string vector
 * Output: inode_ptr to the specified directory
 */
inode_ptr validPath(inode_ptr& directoryPath, const wordvec& words) {
   // Initialize pointer to the cwd
   // inode_ptr finalCwdPtr = state.getCwd();
   inode_ptr finalCwdPtr = directoryPath;

   // Split the pathname into a wordvec
   wordvec tempPath = split(words.at(1), "/");

   // Check to see if the pathname only contains one element
   if (tempPath.size() != 1) {
      tempPath = wordvec(tempPath.begin(), tempPath.end() - 1);
   }

   // Get the directory path minus the last pathname
   // tempPath = wordvec(tempPath.begin(), tempPath.end() - 1);

   // Loop through each path, throw an error if a path doesn't exist
   for (wordvec::const_iterator wordIter = tempPath.begin();
        wordIter != tempPath.end(); ++wordIter)
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

/**
 * Determines the file type
 * Input: indoe_ptr to a directory, wordvec with a pathname
 * Output: inode_ptr to the file/directory
 */
inode_ptr determineFileType(inode_ptr& inodePtr, const wordvec &words) {
   // Initialize return tuple with the inode_ptr to the file/directory
   // and the filetype of the tuple
   // tuple<inode_ptr, file_type> returnTuple;
   
   // Get the pathname
   wordvec tempPath = split(words.at(0), "/");

   // Look for the specified element in dirents with the specified
   // inodePtr
   map<string, inode_ptr> tempDirents = inodePtr->getContents()
      ->getDirents();
   map<string, inode_ptr>::iterator iter =
      tempDirents.find(tempPath.back());

   if (iter == tempDirents.end()) { // file wasn't found
      throw command_error(words.at(0) + ": No such file or directory");
   }

   // If the element is found, update the returnTuple
   // get<0>(returnTuple) = iter->second;
   // get<1>(returnTuple) = iter->second->getFileType();

   // Return the ptr to the element
   return iter->second;
}

void printDirectoryContent(inode_ptr &directoryPtr) {
   // Set a temporary var to class member dirents
   map<string, inode_ptr> tempDirents =
       directoryPtr->getContents()->getDirents();
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
