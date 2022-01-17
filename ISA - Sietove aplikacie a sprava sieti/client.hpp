/**
 *  @file client.hpp
 *  @author Samuel Valaštín, <xvalas10@stud.fit.vutbr.cz>
*/

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <unistd.h>

struct commandLineArgs
{
    std::string hostname = "::1";
    std::string port = "32323";
    std::string command;
    bool register_flag = false;
    bool login_flag = false;
    bool list_flag = false;
    bool send_flag = false;
    bool fetch_flag = false;
    bool logout_flag = false;
    std::string username;
    std::string password;
    std::string recipient;
    std::string subject;
    std::string body;
    int fetch_id;
};