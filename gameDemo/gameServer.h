#pragma once
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <pthread.h>
#include "User.pb.h"

enum RequestType{
    SIGN_IN = 101,  //注册
    LOGIN = 102,    //登陆
    
    OK = 200,       //注册成功
    FAIL = 201,     //注册失败
    LOGINOK = 202,  //登陆成功
    LOGINFAIL = 203 //登陆失败
};

struct UserInfo{
    std::string user_id;
    std::string user_pwd;
    std::string user_name;
};

