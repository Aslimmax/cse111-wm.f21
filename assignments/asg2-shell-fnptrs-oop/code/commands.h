// $Id: commands.h,v 1.17 2021-10-31 02:01:41-07 - - $

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <unordered_map>
using namespace std;

#include "file_sys.h"
#include "util.h"

// A couple of convenient usings to avoid verbosity.

using command_fn = void (*)(inode_state& state, const wordvec& words);
using command_hash = unordered_map<string,command_fn>;

// command_error -
//    Extend runtime_error for throwing exceptions related to this 
//    program.

class command_error: public runtime_error {
   public: 
      explicit command_error (const string& what);
};

// execution functions -

void fn_cat    (inode_state& state, const wordvec& words);
void fn_cd     (inode_state& state, const wordvec& words);
void fn_echo   (inode_state& state, const wordvec& words);
void fn_exit   (inode_state& state, const wordvec& words);
void fn_ls     (inode_state& state, const wordvec& words);
void fn_lsr    (inode_state& state, const wordvec& words);
void fn_make   (inode_state& state, const wordvec& words);
void fn_mkdir  (inode_state& state, const wordvec& words);
void fn_prompt (inode_state& state, const wordvec& words);
void fn_pwd    (inode_state& state, const wordvec& words);
void fn_rm     (inode_state& state, const wordvec& words);
void fn_rmr    (inode_state& state, const wordvec& words);

// helper functions
inode_ptr validPath(inode_ptr& directoryPtr, const wordvec& words);
inode_ptr determineFileType(inode_ptr& inodePtr, const wordvec& words);
void printDirectoryContent(inode_ptr& inodePtr); 

void recursiveRemove(inode_ptr& directoryPtr);
void recursiveRemoveDir(inode_ptr& directoryPtr);
void recursiveLs(inode_state& state, inode_ptr& directoryPtr, 
   const wordvec& words);

command_fn find_command_fn(const string &command);

// exit_status_message -
//    Prints an exit message and returns the exit status, as recorded
//    by any of the functions.

int exit_status_message();
class ysh_exit: public exception {};

#endif

