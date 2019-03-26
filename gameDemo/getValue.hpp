#include "gameServer.h"
void getUser(const std::string& serialized, UserInfo& user)
{
    proto_User::User deserial;
    if(!deserial.ParseFromString(serialized))
    {
        std::cerr << "Failed to parse UserMseeage" << std::endl;
        return;
    }
    std::cout << deserial.DebugString();
    user.user_id = deserial.id();
    user.user_pwd = deserial.password();
    user.user_name = deserial.name();
}

void getEmbattle(const std::string&serialized, EMbattle& em)
{
    proto_User::EMbattle deserial;
    if(!deserial.ParseFromString(serialized))
    {
        std::cerr << "Failed to parse EmbattleMessage" << std::endl;
        return;
    }
    //std::cout << deserial.DebugString();
    for(int i=0; i<10; ++i)
    {
        em.embattle[i] = deserial.embattle(i);
    }
}

