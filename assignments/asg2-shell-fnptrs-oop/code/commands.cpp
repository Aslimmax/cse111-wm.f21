// $Id: commands.cpp,v 1.24 2021-10-27 17:42:59-07 - - $

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

void fn_cat (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
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

   // Loop through dirents
   map<string, inode_ptr> dirents = 
      state.getCwd()->getContents()->getDirents();
   for (map<string, inode_ptr>::iterator iter = dirents.begin(); 
      iter != dirents.end(); ++iter) {
      cout << setw(6) << iter->second->get_inode_nr()
         << setw(6) << iter->second->getContents()->getDirents().size();

      string filePath = iter->first;
      if (filePath == "..") {
         cout << setw(7);
      } else {
         cout << setw(6);
      }

      // Print out the file path
      cout << filePath << endl;
   }
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
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

