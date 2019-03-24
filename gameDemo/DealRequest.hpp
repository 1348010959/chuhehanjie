#pragma once
#include "gameServer.h"

void lengthToByte(unsigned short& len, char buf[])
{
        char* ptr = (char*)&len;
            buf[0] = ptr[0];
                buf[1] = ptr[1];
                
}

void byteToLength(unsigned short& len, char buf[])
{
        char* ptr = (char*)&len;
            ptr[0] = buf[0];
                ptr[1] = buf[1];
                
}

void Sign_in(const UserInfo& user, const int& client_fd)
{
    char userid[32] = {0};
    char name[32] = {0};
    char password[32] = {0};
    int input[2];
    int output[2];

    pipe(input);
    pipe(output);

    pid_t id = fork();
    if(id < 0){
        std::cerr << "fork" << std::endl;
        return;
    }
    else if( id == 0 ){
        close(input[1]);
        close(output[0]);

        dup2(input[0], 0);  //对子进程的标准输入输出进行重定向
        dup2(output[1], 1);

        sprintf(userid, "USERID=%s", user.user_id.c_str());
        putenv(userid);
        sprintf(name, "NAME=%s", user.user_name.c_str());
        putenv(name);
        sprintf(password, "PASSWORD=%s", user.user_pwd.c_str());
        putenv(password);
        execl("./go_sql/Sign_in", NULL);
        exit(1);
    }else{
        close(input[0]);
        close(output[1]);

        char buf[64] = {0};
        char msg[8] = {0};
        if(read(output[0], buf, sizeof(buf)-1) < 0)
        {
            std::cerr << "read" << std::endl;
        }
        if(strcasecmp(buf, "OK") == 0)
        {
            std::cout << "SIGN_IN success" << std::endl;
            msg[0] = OK;
        }else{
            std::cout << "SIGN_IN fail" << std::endl;
            msg[0] = FAIL;
        }
        printf("sql:%s\n", buf);
        write(client_fd, msg, 1);
    }
}

void Login(const UserInfo& user, const int& client_fd, std::map<unsigned int, OnlineInfo>& online)
{
    char userid[32] = {0};
    char password[32] = {0}; 
    int input[2];
    int output[2];

    pipe(input);
    pipe(output);

    OnlineInfo tmp;
    tmp.user_id = user.user_id;
    tmp.Isplaying = false;

    pid_t id = fork();
    if(id < 0){
        std::cerr << "fork" << std::endl;
        return;
    }
    if( id == 0 ){
        close(input[1]);
        close(output[0]);

        dup2(input[0], 0);  //对子进程的标准输入输出进行重定向
        dup2(output[1], 1);

        sprintf(userid, "USERID=%s", user.user_id.c_str());
        putenv(userid);
        sprintf(password, "PASSWORD=%s", user.user_pwd.c_str());
        putenv(password);
        if(execl("./go_sql/Login", NULL) < 0)
        {
            std::cout<<"execl"<<std::endl;
            exit(1);
        }
    }else{
        close(input[0]);
        close(output[1]);

        char buf[256] = {0};
        char msg[256] = {0};
        EMbattle em;
        proto_User::EMbattle Em;
        std::string serial;
        int size = read(output[0], buf, sizeof(buf)-1);
        if(size < 0)
        {
            std::cerr << "read" << std::endl; 
        }
        if((strncasecmp(buf, "OK", 2) == 0))
        {
            printf("It's is success\n");
            msg[0] = LOGINOK;
            for(int i=0; i<10; ++i)
            {
                char temp[10];
                memcpy(temp, (buf+2+i*10), 10);
                em.embattle[i] = temp;
                *Em.add_embattle() = em.embattle[i];
            }
            em.name = buf+102;
            Em.set_name(em.name);
            Em.SerializeToString(&serial);
            unsigned short len = serial.size();
            char lenbuf[2] = {0};
            lengthToByte(len, lenbuf);
            msg[1] = lenbuf[0];
            msg[2] = lenbuf[1];
            strcpy(msg+3, serial.c_str());
            online.insert(std::pair<unsigned int, OnlineInfo> (client_fd, tmp));
            std::cout << online.size() << std::endl;
        }else{
            printf("It's is failed\n");
            msg[0] = LOGINFAIL;
        }
        printf("sql:%s\n", buf);
        printf("msg:%s\n", msg+1);
        write(client_fd, msg, sizeof(msg)-1);
    }
}

void Embattle(EMbattle& em, const unsigned int& client_fd, std::map<unsigned int, OnlineInfo>& online)
{
    char Tag[128] = {0};
    char Id[32] = {0};
    int input[2];
    int output[2];
    /*std::list<OnlineUser>::iterator it = online.begin();
      bool flag = false;
      while(it != online.end())
      {
      if((*it).sock_fd == client_fd)
      {
      flag = true;
      break;
      }
      it++;
      }*/
    /*if(!flag)
      {
      std::cout << "Not online User, error!!" << std::endl;
      }*/

    //char tagstr[10] = {0};
    std::string tag = "";
    for(int i=0; i<10; i++)
    {
        /*std::cout << em.embattle[i].size() << std::endl;
          memcpy(tagstr, em.embattle[i].c_str(), 10);
          tag += tagstr;
          memset(tagstr, 0, 10);*/
        tag += em.embattle[i].substr(0,10);
        //std::cout << tag.size() << std::endl;
    }

    //if(flag)                                                                                                                                              
    {
        std::cout << "Args Success" << std::endl;
        sprintf(Id, "USERID=%s", online[client_fd].user_id.c_str());
        putenv(Id);
        sprintf(Tag, "TAG=%s", tag.c_str());
        putenv(Tag);
        printf("%s\n", Tag);
    }

    pipe(input);
    pipe(output);

    pid_t id = fork();

    if(id < 0){
        std::cerr << "fork" << std::endl;
        return;
    }
    if( id == 0  ){
        close(input[1]);
        close(output[0]);

        dup2(input[0], 0);  //对子进程的标准输入输出进行重定向
        dup2(output[1], 1);

        if(execl("./go_sql/Update", NULL) < 0)
        {
            std::cerr << "execl error" << std::endl;
            exit(1);
        }
    }else{
        close(input[0]);
        close(output[1]);

        char msg[8] = {0};
        char buf[256] = {0};
        ssize_t s = read(output[0], buf, sizeof(buf)-1);
        if(s < 0)
        {
            std::cerr << "read" << std::endl;
        }
        if(strncasecmp(buf, "OK", 2) == 0)
        {
            std::cout << "Update success" << std::endl;
            msg[0] = OK;
        }else{
            std::cout << "Update faile" << std::endl;
            msg[0] = FAIL;
        }
        printf("buf:%s\n", buf);
        write(client_fd, msg, sizeof(msg)-1);
    }
}

static bool StartGame(unsigned int playfdA, unsigned int playfdB)
{
    char recvbufA[256] = {0};
    char recvbufB[256] = {0};
    unsigned short size = 0;
    char lenbuf[2] = {0};
    bool flagA, flagB;
    flagA = false;
    flagB = false;
    for( ; ;){
        ssize_t n = read(playfdA, recvbufA, sizeof(recvbufA)-1);
        if( n < 0 ){
            std::cerr << "read playfdA" << std::endl;
            continue;
        }
        if(write(playfdB, recvbufA, n)){
            std::cerr << "write playfdB" << std::endl;
            continue;
        }
        if(recvbufA[0] == GAMEOVER)
        {
            flagA = true;
        }
        memset(recvbufA, 0, sizeof(recvbufA)-1);
        n = read(playfdB, recvbufB, sizeof(recvbufB)-1);
        if( n < 0  ){
            std::cerr << "read playfdB" << std::endl;
            continue;

        }
        if(write(playfdA, recvbufB, n))
        {
            std::cerr << "write playfdA" << std::endl;
        }
        if(recvbufB[0] == GAMEOVER)
        {
            flagB = true;
        }
        memset(recvbufB, 0, sizeof(recvbufA)-1);
        if(flagA && flagB)
            break;
    }
    return true;
}

void* Match(void* parg)
{
    //std::list<OnlineUser>::iterator it = online.begin();
    /*for( ; it != online.end(); ++it )
      {
      if( (*it).sock_fd == client_fd ){
      MatchQueue.push(*it);
      }
      }*/
    Args* ptr = (Args*)parg;
    pthread_mutex_lock(&mutex);
    ptr->MatchQueue.push(ptr->client_fd);
    if(ptr->MatchQueue.size() % 2 == 0)
    {
        //pthread_cond_signal(&cond);
        char msg[8] = {0};
        unsigned int playfdA = ptr->MatchQueue.front();
        msg[0] = RED;
        write(playfdA, msg, 1);
        ptr->online[playfdA].Isplaying = true;
        ptr->MatchQueue.pop();
        unsigned int playfdB = ptr->MatchQueue.front();
        msg[0] = BLUE;
        write(playfdA, msg, 1);
        ptr->online[playfdB].Isplaying = true;
        ptr->MatchQueue.pop();
        bool play = StartGame(playfdA, playfdB);
        if(play){
            ptr->online[playfdA].Isplaying = false;
            ptr->online[playfdB].Isplaying = false;
        }
    }
    pthread_mutex_unlock(&mutex);
    //ptr->MatchQueue.push(ptr->online[ptr->client_fd]);
    //int player_fd = ptr->client_fd;
    //unsigned int size = ptr->MatchQueue.size();
    /*while(true){
      if(size % 2 == 0){
      StartGame(ptr->MatchQueue.pop, ptr->MatchQueue.pop);
      break;
      }
      }*/

    delete ptr;
    return NULL;
}

