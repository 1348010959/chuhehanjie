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
