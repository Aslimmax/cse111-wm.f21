mkdir test
cd test
make file.txt test file
cd ..
cat test/file.txt

bash-4.2$ ls
assignments
bash-4.2$ cd ~
bash-4.2$ ls
OldFiles  Test.txt  Untitled.txt  cse111-wm.f21  public_html  welcome
bash-4.2$ cat Test.txt Untitled.txt
This is antoher test file where I am going to demonstrate what happens when I cat 2 files at once
This is a test file called Untitled.txt
bash-4.2$ 

g++ -std=gnu++2a -g -O0 -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast -fdiagnostics-color=never -c main.cpp
In file included from main.cpp:11:
commands.h:44:13: warning: ‘void printDirectoryContent(inode_ptr&)’ declared ‘static’ but never defined [-Wunused-function]
 static void printDirectoryContent(inode_ptr& inodePtr);
             ^~~~~~~~~~~~~~~~~~~~~

==4247==    definitely lost: 48 bytes in 1 blocks
==4247==    indirectly lost: 237 bytes in 5 blocks