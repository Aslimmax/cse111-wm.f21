// $Id: cxi.cpp,v 1.6 2021-11-08 00:01:44-08 - - $
// Andrew Lim (ansclim@ucsc.edu)
// Cody Yiu   (coyiu@ucsc.edu)

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#include "debug.h"
#include "logstream.h"
#include "protocol.h"
#include "socket.h"

logstream outlog (cout);
struct cxi_exit: public exception {};

unordered_map<string,cxi_command> command_map {
   {"exit", cxi_command::EXIT},
   {"help", cxi_command::HELP},
   {"ls"  , cxi_command::LS  },
   {"get" , cxi_command::GET },
   {"put" , cxi_command::PUT },
   {"rm"  , cxi_command::RM  },
};

static const char help[] = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cxi_help() {
   cout << help;
}

void cxi_ls (client_socket& server) {
   cxi_header header;
   header.command = cxi_command::LS;
   DEBUGF ('h', "sending header " << header << endl);
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   DEBUGF ('h', "received header " << header << endl);
   if (header.command != cxi_command::LSOUT) {
      outlog << "sent LS, server did not return LSOUT" << endl;
      outlog << "server returned " << header << endl;
   }else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      recv_packet (server, buffer.get(), host_nbytes);
      DEBUGF ('h', "received " << host_nbytes << " bytes");
      buffer[host_nbytes] = '\0';
      cout << buffer.get();
   }
}

/**
 * cxi_get (client_socket& server, string filename)
 * Copy the file named filename on the remote server and create or
 * override a file of the same name in the current directory
 * Input: client_socket server, string filename
 * Output: none
 */
void cxi_get (client_socket& server, string filename) {
   // Initialize a cxi_header object to send to the server
   cxi_header header;
   header.command = cxi_command::GET;

   // Copy filename (string) into header (C str)
   strcpy(header.filename, filename.c_str());

   DEBUGF ('h', "sending header " << header << endl);
   // Send header to server
   send_packet(server, &header, sizeof header);
   // Receive header from server
   recv_packet(server, &header, sizeof header);
   DEBUGF ('h', "received header " << header << endl);
   
   // Check if an error was encountered when the header was sent
   if (header.command != cxi_command::FILEOUT) {
      outlog << "sent GET, server did not return FILEOUT" << endl;
      outlog << "server returned " << header << endl;
   } else {
      size_t host_nbytes = ntohl (header.nbytes);
      auto buffer = make_unique<char[]> (host_nbytes + 1);
      // Check if the payload that will be receieved is empty
      if (host_nbytes > 0) {      
         recv_packet (server, buffer.get(), host_nbytes);
      }
      DEBUGF ('h', "received " << host_nbytes << " bytes");
      buffer[host_nbytes] = '\0';

      // Initialize an ofstream object to create a file named filename
      bool fileStatus = true;
      writeFile (buffer.get(), filename, host_nbytes, fileStatus);

      outlog << "file '" << filename << "' retrieved from server" 
         << endl;
   }
}

/**
 * Todo(ansclim, coyiu)
 * cxi_put (client_socket& server, string filename)
 * Copies a local file into the socket and causes the remote server to
 * create that file in its directory
 * Input: client_socket server, string filename
 * Output: none
 */
void cxi_put (client_socket& server, string filename) {
   // Initialize a cxi_header object to send to the server
   cxi_header header;
   header.command = cxi_command::PUT;

   // Process the filename via stat
   struct stat stat_buf;
   int status = stat (filename.c_str(), &stat_buf);
   // Get the filesize  
   int filesize = stat_buf.st_size;

   // Check if the filename passed exists
   if (status != 0) {
      outlog << filename << ": " << strerror(errno) << endl;
      return;
   }

   header.nbytes = htonl(filesize);
   // Copy filename (string) into header (C str)
   strcpy(header.filename, filename.c_str());

   // Send header to server
   DEBUGF ('h', "sending header " << header << endl);
   send_packet (server, &header, sizeof header);

   // If the file is empty, don't send a payload to the server
   if (filesize > 0) {
      // Initialize a buffer to store the contents of filename
      char* buffer = new char [filesize + 1];
      const char *cfilename = filename.c_str();

      // Read filename's contents into buffer
      bool fileStatus = true;
      readFile (buffer, cfilename, filesize, fileStatus);
      buffer[filesize] = '\0';
      // Send payload object to server
      DEBUGF ('h', "sending payload " << header << endl);
      send_packet (server, buffer, filesize);

      // Free memory
      delete[] buffer;   
   }

   // Receive header from server
   recv_packet (server, &header, sizeof header);
   if (header.command != cxi_command::ACK) {
      outlog << "sent PUT, server did not return ACK" << endl;
      outlog << "server returned " << header << endl;
   } else {
      outlog << "file '" << filename 
         << "' was successfully copied to the server" << endl;
   }
}

/**
 * Todo(ansclim, coyiu)
 * cxi_rm (client_socket server, string filename)
 * Causes the remote server to remove the file
 * Input: client_socket server, string filename
 * Output: none
 */
void cxi_rm (client_socket& server, string filename) {
   // Initialize a cxi_header object to send to the server
   cxi_header header;
   header.command = cxi_command::RM;
   // Copy filename (string) into header (C str)
   strcpy(header.filename, filename.c_str());

   DEBUGF ('h', "sending header " << header << endl);
   // Send header to server
   send_packet (server, &header, sizeof header);
   // Receive header from server
   recv_packet (server, &header, sizeof header);
   DEBUGF ('h', "received header " << header << endl);

   // Check if an error was encountered when the file was attempted to
   // be removed
   if (header.command == cxi_command::NAK) {
      outlog << "sent RM, server did not return ACK" << endl;
      outlog << "server returned " << header << endl;
   } else if (header.command == cxi_command::ACK) {
      outlog << "file '" << filename 
         << "' was successfully removed from the server" << endl;
   }
}


void usage() {
   cerr << "Usage: " << outlog.execname() << " host port" << endl;
   throw cxi_exit();
}

pair<string,in_port_t> scan_options (int argc, char** argv) {
   for (;;) {
      int opt = getopt (argc, argv, "@:");
      if (opt == EOF) break;
      switch (opt) {
         case '@': debugflags::setflags (optarg);
                   break;
      }
   }
   if (argc - optind != 2) usage();
   string host = argv[optind];
   in_port_t port = get_cxi_server_port (argv[optind + 1]);
   return {host, port};
}

int main (int argc, char** argv) {
   outlog.execname (basename (argv[0]));
   outlog << to_string (hostinfo()) << endl;
   try {
      auto host_port = scan_options (argc, argv);
      string host = host_port.first;
      in_port_t port = host_port.second;
      outlog << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      outlog << "connected to " << to_string (server) << endl;
      for (;;) {
         // Initailize strings to store command and filename
         string command = "";
         string filename = "";
         // Store arguments in a vector for additional processing
         vector<string> clientArgs;
         // Get command and arguments from client and convert to a
         // string stream object
         string line = "";
         getline(cin, line);
         istringstream iss(line);
         if (cin.eof()) throw cxi_exit();
         
         // Store each split argument
         string item{};
         // Iterate the split data and push data to vector
         while (getline(iss, item, ' ')) {
            clientArgs.push_back(item);
         }

         // Check if arguments were passed
         if (!clientArgs.empty()) {
            command = clientArgs[0];
            // Check if a filename was passed
            if (clientArgs.size() == 2) {
               filename = clientArgs[1];
            }
         }
         outlog << "command " << command << endl;

         // Determine if the command passed is a valid command defined
         // in command_map
         const auto& itor = command_map.find (command);
         cxi_command cmd = itor == command_map.end()
                         ? cxi_command::ERROR : itor->second;

         if (clientArgs.size() > 1 && (cmd == cxi_command::EXIT ||
            cmd == cxi_command::HELP || cmd == cxi_command::LS)) {
            outlog << line << ": too many arguments" << endl;
            continue;
         } else if (cmd == cxi_command::GET
            || cmd == cxi_command::PUT || cmd == cxi_command::RM) {
            if (clientArgs.size() > 2) {
               outlog << line << ": too many arguments" << endl;
               continue;
            } else if (clientArgs.size() < 2) {
               outlog << line << ": too few arguments" << endl;
               continue;
            } else {
               if (clientArgs[1].length() > 58) {
                  outlog << line << ": filename too long" << endl;
                  continue;
               } else if (clientArgs[1].find('/') != string::npos) {
                  outlog << line << ": filename has '/'" << endl;
                  continue;
               }
            }
         }

         switch (cmd) {
            case cxi_command::EXIT:
               throw cxi_exit();
            case cxi_command::HELP:
               cxi_help();
               break;
            case cxi_command::LS:
               cxi_ls (server);
               break;
            case cxi_command::GET:
               cxi_get (server, filename);
               break;
            case cxi_command::PUT:
               cxi_put (server, filename);
               break;
            case cxi_command::RM:
               cxi_rm (server, filename);
               break;
            default:
               outlog << line << ": invalid command" << endl;
         }
      }
   }catch (socket_error& error) {
      outlog << error.what() << endl;
   }catch (cxi_exit& error) {
      DEBUGF ('x', "caught cxi_exit");
   }
   return 0;
}

