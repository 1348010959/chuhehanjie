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

void Sign_in(const UserInfo& user, const unsigned int& client_fd)
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

void Login(const UserInfo& user, const unsigned int& client_fd, std::list<OnlineUser>& online)
{
    char userid[32] = {0};
    char password[32] = {0}; 
    int input[2];
    int output[2];

    pipe(input);
    pipe(output);

    /*OnlineInfo tmp;
      tmp.user_id = user.user_id;
      tmp.Isplaying = false;*/
    OnlineUser tmp;
    tmp.sock_fd = client_fd;
    tmp.user_id = user.user_id;
    tmp.Isplaying = false;

    std::cout << "进入登陆函数" << std::endl;

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
            //online.insert(std::make_pair(client_fd, tmp));
            //pthread_mutex_lock(&mutex);
            online.push_back(tmp);
            std::cout << "online size" << online.size() << std::endl;
            //pthread_mutex_unlock(&mutex);
            //std::cout << online[client_fd].user_id << std::endl;
        }else{
            printf("It's is failed\n");
            msg[0] = LOGINFAIL;
        }
        //printf("sql:%s\n", buf);
        printf("msg:%s\n", msg+3);
        write(client_fd, msg, sizeof(msg)-1);
    }
}

void Embattle(EMbattle& em, const unsigned int& client_fd, std::list<OnlineUser>& online)//std::map<unsigned int, OnlineInfo>& online)
{
    char Tag[128] = {0};
    char Id[32] = {0};
    int input[2];
    int output[2];
    std::list<OnlineUser>::iterator it = online.begin();
    bool flag = false;
    while(it != online.end())
    {
        if((*it).sock_fd == client_fd)
        {
            flag = true;
            break;
        }
        it++;
    }
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

    if(flag)                                                                                                                                              
    {
        std::cout << "Args Success" << std::endl;
        std::cout << "userid : " << (*it).user_id << std::endl;
        sprintf(Id, "USERID=%s", (*it).user_id.c_str());//online[client_fd].user_id.c_str());
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

void Broadcast(const unsigned int& playfdA, const unsigned int& playfdB, const bool& troop, bool broadcast , char* buf, const int& n)
{
    //static int count = 0;
    static char IsReady[64] = {0};
    if(troop){
        IsReady[playfdA%61]++;
        //count++;
        std::cout << "IsReady:" << (int)IsReady[playfdA%61] <<"broadcast :" << broadcast <<std::endl;
    }
    if(IsReady[playfdA%61] == 2){//(count == 2){ //&& IsReady[playfdA%61] == 2){
        broadcast = true;
        buf[0] = NEWROUND;
        //count = 0;
        IsReady[playfdA%61] = 0;
    }
    if(broadcast){
        if(write(playfdB, buf, n) < 0){
            std::cerr << "write playfdB" << std::endl;
        }
        if(write(playfdA, buf, n) < 0)
        {
            std::cerr << "write playfdA" << std::endl;
        }
        std::cout << "write A and B success" << std::endl;
    }else{
        std::cout << "write A and B fail" << std::endl;
    }
    memset(buf, 0, 256);
}

void* StartGameA(void* fd)
{
    Args arg = *(Args*)fd;
    const unsigned int playfdA = arg.client_fd[0];
    const unsigned int playfdB = arg.client_fd[1];
    const unsigned int epoll_fd = arg.client_fd[2];

    char recvbuf[256] = {0};
    ssize_t temp = read(playfdA, recvbuf, sizeof(recvbuf)-1);
    if(temp >0 && recvbuf[0] == ENEMY)
    {
        std::cout << "send enemy EMbattle" << std::endl;
        write(playfdB, recvbuf, temp);
    }else if(temp == 0){
        std::list<OnlineUser>::iterator it = (*(arg.online)).begin();
        for( ; it != (*(arg.online)).end(); ++it    )
        {   
            if( (*it).sock_fd == playfdA    ){
                (*it).Isplaying = false;
                close(playfdA);
                //pthread_mutex_lock(&mutex);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, playfdA, NULL);
                (*(arg.online)).erase(it);
                //pthread_mutex_unlock(&mutex);
                std::cout << " A client quit" << std::endl;                                                                                     
                std::cout << "OnlineUser :" << (*(arg.online)).size() << std::endl;
                break;
            }
        }
        delete (Args*)fd;
        fd = NULL;
        return NULL;
    }
    memset(recvbuf, 0, sizeof(recvbuf)-1);
    //char sendbufA[256] = {0};
    //char recvbufB[256] = {0};
    //char sendbufB[256] = {0};
    //unsigned short size = 0;
    //char lenbuf[2] = {0};
    for( ; ;){
        ssize_t n = read(playfdA, recvbuf, 256);
        if( n < 0 ){
            std::cerr << "read playfdA" << std::endl;
            continue;
        }
        std::cout << "read playfdA size #" << n << std::endl;
        bool troop, broadcast;
        if(n > 0)
        {
            if(recvbuf[0] == READY){
                std::cout << "A 请求出兵" << std::endl;
                troop = true;
                broadcast = false;
            }else{
                broadcast = true;
                troop = false;
            }
            Broadcast(playfdA, playfdB, troop, broadcast, recvbuf, n);
        }
        /*if( n > 0 ){
          pthread_mutex_lock(&mutex);
          if(write(playfdB, recvbufA, n) < 0){
          std::cerr << "write playfdB" << std::endl;
          }
          if(write(playfdA, recvbufA, n) < 0)
          {
          std::cerr << "write playfdA" << std::endl;
          }
          pthread_mutex_unlock(&mutex);
          std::cout << "write A and B success" << std::endl;
          }*/
        if(n == 0 || recvbuf[0] == REDWIN || recvbuf[0] == BLUEWIN || recvbuf[0] == NONEWIN){
            //pthread_mutex_lock(&mutex);
            std::list<OnlineUser>::iterator it = (*(arg.online)).begin();
            for( ; it != (*(arg.online)).end(); ++it   )
            {
                if( (*it).sock_fd == playfdA   ){
                    (*it).Isplaying = false;                                                                    
                    break;
                }                    
            }                        
            if(n == 0){
                close(playfdA);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, playfdA, NULL);
                (*(arg.online)).erase(it);
                std::cout << "A client quit" << std::endl;                                                                                     
                std::cout << "OnlineUser :" << (*(arg.online)).size() << std::endl;
                //pthread_mutex_unlock(&mutex);              
            }else{
                recvbuf[0] = GAMEOVER;
                broadcast = false;
                troop = true;
                Broadcast(playfdA, playfdB, troop, broadcast, recvbuf, n);
            }
            //pthread_mutex_unlock(&mutex);
            break;
        }
    }
    /*n = read(playfdB, recvbufB, sizeof(recvbufB)-1);
      std::cout << "read playfdB size #" << n << std::endl;
      if( n < 0  ){
      std::cerr << "read playfdB" << std::endl;
      continue;
      }
      if( n > 0 ){
      if(write(playfdA, recvbufB, n) < 0)
      {
      std::cerr << "write playfdA" << std::endl;
      }
      if(write(playfdA, recvbufB, n) < 0)
      {
      std::cerr << "write playfdA" << std::endl;
      }

      std::cout << "write A and B success" << std::endl;
      }
      if(recvbufB[0] == GAMEOVER)
      {
      flagB = true;
      }
      memset(recvbufB, 0, sizeof(recvbufA)-1);*/
    //return true;
    delete (Args*)fd;
    fd = NULL;
    return NULL;
}

void* StartGameB(void* fd)
{
    Args arg = *(Args*)fd;
    const unsigned int playfdA = arg.client_fd[0];
    const unsigned int playfdB = arg.client_fd[1];
    const unsigned int epoll_fd = arg.client_fd[2];
    char recvbuf[256] = {0};
    ssize_t temp = read(playfdB, recvbuf, sizeof(recvbuf)-1);
    if(temp >0 && recvbuf[0] == ENEMY)
    {
        std::cout << "send enemy EMbattle" << std::endl;
        write(playfdA, recvbuf, temp);
    }else if(temp == 0){
        std::list<OnlineUser>::iterator it = (*(arg.online)).begin();
        for( ; it != (*(arg.online)).end(); ++it   )
        {
            if( (*it).sock_fd == playfdB   ){
                (*it).Isplaying = false;
                close(playfdB);
                //pthread_mutex_lock(&mutex);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, playfdB, NULL);
                (*(arg.online)).erase(it);
                //pthread_mutex_unlock(&mutex);
                std::cout << "B client quit" << std::endl;                                                                                     
                std::cout << "OnlineUser :" << (*(arg.online)).size() << std::endl;
                break;
            }
        }
        return NULL;
    }

    memset(recvbuf, 0, 256);
    for( ; ; ){ 
        ssize_t n = read(playfdB, recvbuf, 256);
        if( n < 0  ){
            std::cerr << "read playfdB" << std::endl;
            continue;
        }      
        std::cout << "read playfdB size #" << n << std::endl;
        bool troop, broadcast;

        if( n > 0 ){
            if(recvbuf[0] == READY){
                std::cout << "B 请求出兵" << std::endl;
                troop = true;
                broadcast = false;
            }else{
                broadcast = true;
                troop = false;
            }
            Broadcast(playfdA, playfdB, troop, broadcast, recvbuf, n);
        }
        /*if( n > 0  ){
          pthread_mutex_lock(&mutex);
          if(write(playfdB, recvbuf, n) < 0){
          std::cerr << "write playfdB" << std::endl;
          }  
          if(write(playfdA, recvbuf, n) < 0)
          {  
          std::cerr << "write playfdB" << std::endl;

          }  
          pthread_mutex_unlock(&mutex);
          std::cout << "write A and B success" << std::endl;
          }*/      
        if(n == 0 || recvbuf[0] == REDWIN || recvbuf[0] == BLUEWIN || recvbuf[0] == NONEWIN){
            //pthread_mutex_lock(&mutex);
            std::list<OnlineUser>::iterator it = (*(arg.online)).begin();
            for( ; it != (*(arg.online)).end(); ++it  )
            {
                if( (*it).sock_fd == playfdB  ){
                    (*it).Isplaying = false;
                    std::cout << "跳出在线列表" << std::endl;
                    break;
                }
            }
            if(n == 0){
                close(playfdB);
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, playfdB, NULL);
                (*(arg.online)).erase(it);
                std::cout << "B client quit" << std::endl;
                std::cout << "OnlineUser :" << (*(arg.online)).size() << std::endl;
                //pthread_mutex_unlock(&mutex);
            }else{
                recvbuf[0] = GAMEOVER;
                broadcast = false;
                troop = true;
                Broadcast(playfdA, playfdB, troop, broadcast, recvbuf, n);
            }
            //pthread_mutex_unlock(&mutex);
            break;
        }
    }
    return NULL;
}


void Match(std::list<OnlineUser>&online, std::queue<unsigned int>& MatchQueue, const unsigned int& client_fd, threadpool_t* pool, const unsigned int& epoll_fd)
{
    std::list<OnlineUser>::iterator it = online.begin();
    for( ; it != online.end(); ++it )
    {
        if( (*it).sock_fd == client_fd ){
            MatchQueue.push(it->sock_fd);
        }
    }
    //pthread_mutex_lock(&mutex);
    if(MatchQueue.size() >= 2)
    {
        char msg[8] = {0};
        unsigned int playfdA = MatchQueue.front();
        msg[0] = RED;
        write(playfdA, msg, 1);
        std::cout << "A为红方" << std::endl;
        MatchQueue.pop();
        unsigned int playfdB = MatchQueue.front();
        msg[0] = BLUE;
        write(playfdB, msg, 1);
        std::cout << "B为蓝方" << std::endl;
        MatchQueue.pop();
        it = online.begin();
        int count = 0;
        for( ; it != online.end(); ++it )
        {
            if( (*it).sock_fd == playfdA  ){
                it->Isplaying = true;
                count++;
            }
            if( (*it).sock_fd == playfdB ){
                it->Isplaying = true;
                count++;
            }
            if(count == 2)
                break;
        }
        //unsigned int fd[2] = {playfdA, playfdB};
        Args* arg = new Args;
        arg->client_fd[0] = playfdA;
        arg->client_fd[1] = playfdB;
        arg->client_fd[2] = epoll_fd;
        arg->online = &online;
        thread_add_task(pool, StartGameA, arg);
        thread_add_task(pool, StartGameB, arg);
    }
    //pthread_mutex_unlock(&mutex);

    //Args* ptr = (Args*)parg;
    /*pthread_mutex_lock(&mutex);
      ptr->MatchQueue.push(ptr->client_fd);
      std::cout << ptr->MatchQueue.size() << std::endl;
      pthread_mutex_unlock(&mutex);*/
    /*if(ptr->MatchQueue.size() >= 2)
      {
    //pthread_cond_signal(&cond);
    char msg[8] = {0};
    unsigned int playfdA = ptr->MatchQueue.front();
    pthread_mutex_lock(&mutex);
    msg[0] = RED;
    write(playfdA, msg, 1);
    ptr->online[playfdA].Isplaying = true;
    ptr->MatchQueue.pop();
    unsigned int playfdB = ptr->MatchQueue.front();
    msg[0] = BLUE;
    write(playfdB, msg, 1);
    ptr->online[playfdB].Isplaying = true;
    ptr->MatchQueue.pop();
    bool play = StartGame(playfdA, playfdB);
    if(play){
    ptr->online[playfdA].Isplaying = false;
    ptr->online[playfdB].Isplaying = false;
    }
    pthread_mutex_unlock(&mutex);
    }*/
    //ptr->MatchQueue.push(ptr->online[ptr->client_fd]);
    //int player_fd = ptr->client_fd;
    //unsigned int size = ptr->MatchQueue.size();
    /*while(true){
      if(size % 2 == 0){
      StartGame(ptr->MatchQueue.pop, ptr->MatchQueue.pop);
      break;
      }
      }*/
}

