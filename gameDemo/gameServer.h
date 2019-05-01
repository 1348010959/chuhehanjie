#pragma once
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <linux/types.h>
#include <asm/byteorder.h>
//#include <linux/config.h>
//#include <linux/skbuff.h>
//#include <linux/ip.h>
//#include <net/sock.h>
//#include <linux/tcp.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <ctype.h>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <pthread.h>
#include <list>
#include <queue>
//#include <map>
#include "threadpool.h"
#include "User.pb.h"

enum RequestType{
    SIGN_IN = 101,  //注册
    LOGIN = 102,    //登陆
    
    OK = 200,       //注册成功
    FAIL = 201,     //注册失败
    LOGINOK = 202,  //登陆成功
    LOGINFAIL = 203, //登陆失败

    START = 16,    //开始游戏
    GAMEOVER = 17,  //结束游戏
    EMBATTLE = 18,      //布阵
    PLAYDATA = 19,  //游戏中玩家数据
    SOILDERDATA = 20, //游戏中士兵数据
    BULLETDATA = 21,

    REDWIN = 95,    //红方胜利
    BLUEWIN = 96,   //蓝方胜利
    NONEWIN = 97,   //平局
    RED = 98,       //红方阵营
    BLUE = 99,      //蓝方阵营

    LOST = 60,     //异常离线
    LOSTACK = 61,  //收到回复    


    READY = 50,  //出兵消息
    NEWROUND = 51,
    ENEMY = 52  //对端数据
};

struct UserInfo{
    std::string user_id;
    std::string user_pwd;
    std::string user_name;
};

struct OnlineInfo{
    std::string user_id;
    bool Isplaying;
};

pthread_mutex_t mutex_online;

struct OnlineUser{
    std::string user_id;
    unsigned int sock_fd;   //在线用户套接字
    //std::map<unsigned int, OnlineInfo> online;
    bool Isplaying;     //玩家是否正在游戏
};

struct Args{
    //std::map<unsigned int, OnlineInfo> online;
    //std::queue<unsigned int> MatchQueue;
    unsigned int client_fd[3];
    std::list<OnlineUser>* online;
};

struct EMbattle{
    std::string name;
    std::string embattle[10];
};

