#pragma once
#include "gameServer.h"
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

void Login(const UserInfo& user, const int& client_fd, std::list<OnlineUser> online)
{
    char userid[32] = {0};
    char password[32] = {0}; 
    int input[2];
    int output[2];

    pipe(input);
    pipe(output);

    OnlineUser tmp;
    tmp.user_id = user.user_id;
    tmp.sock_fd = client_fd;
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
            strcpy(msg+1, serial.c_str());
            online.push_back(tmp);
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

void Embattle(EMbattle& em, const unsigned int& client_fd, std::list<OnlineUser>& online)
{
    char Tag[128] = {0};
    char Id[32] = {0};
    int input[2];
    int output[2];
    char buf[8] = {0};
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
    if(!flag)
    {
        std::cout << "Not online User, error!!" << std::endl;
    }

    std::string tag = em.embattle[0];
    for(int i=1; i<9; ++i)
    {
        tag += em.embattle[i];
    }

    pipe(input);
    pipe(output);

    pid_t id = fork();
    if(id < 0){
        std::cerr << "fork" << std::endl;
        return;
    }
    else if( id == 0  ){
        close(input[1]);
        close(output[0]);

        dup2(input[0], 0);  //对子进程的标准输入输出进行重定向
        dup2(output[1], 1);

        //if(flag)
        {
            sprintf(Id, "UERID=%s", it->user_id.c_str());
            putenv(Id);
            sprintf(Tag, "TAG=%s", tag.c_str());
            putenv(Tag);

            execl("./go_sql/Update", NULL);
            exit(1);
        }
    }else{
        close(input[0]);
        close(output[1]);


        char msg[8] = {0};
        if(read(output[0], buf, sizeof(buf)-1) < 0)
        {
            std::cerr << "read" << std::endl;
        }
        if(strcasecmp(buf, "OK") == 0)
        {
            std::cout << "Update success" << std::endl;
            msg[0] = OK;
        }else{
            std::cout << "Update faile" << std::endl;
            msg[0] = FAIL;
        }
        write(client_fd, msg, sizeof(msg)-1);
    }
}

static void StartGame()
{
}

void Match(std::list<OnlineUser>& online, std::queue<OnlineUser>& MatchQueue, const unsigned int& client_fd)
{
    std::list<OnlineUser>::iterator it = online.begin();
    for( ; it != online.end(); ++it )
    {
        if( (*it).sock_fd == client_fd ){
            MatchQueue.push(*it);
        }
    }
    unsigned int size = MatchQueue.size();
    if(size % 2 == 0){
        StartGame();
    }
}

