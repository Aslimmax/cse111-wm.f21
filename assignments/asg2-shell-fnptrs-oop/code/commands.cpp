// $Id: commands.cpp,v 1.21 2021-09-26 12:41:17-07 - - $

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
Input: inode_state& state, wordvec& words (DEBUG: DEFINE EXTENSIVELY)
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
}

void fn_prompt (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

