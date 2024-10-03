#include "user.hpp"
#include "../session/session.hpp"
#include "../../packets/packets.hpp"

namespace realtime_usd_market{

    User::User(const std::string& l,const size_t& p){
        login = l;
        pswd = p;
    }

    User::~User(){

    }

    std::string User::getLogin(){
        return login;
    }

    void User::setSession(Session* s){
        session = s;
    }

    bool User::hasSession(){
        return session != nullptr;
    }

    bool User::comparePassword(const size_t& p){
        return p == pswd;
    }

    int64_t User::getRub(){
        std::lock_guard<std::mutex> lock(rubMutex);
        return rub;
    }

    int64_t User::getUsd(){
        std::lock_guard<std::mutex> lock(usdMutex);
        return usd;
    }

    void User::addRub(const int64_t& v){
        {
            std::lock_guard<std::mutex> lock(rubMutex);
            rub += v;
        }
        if(session != nullptr){
            if(v > 0){
                AnnounceMessage pk("You earned "+std::to_string(v)+" rub");
                session->sendPacket(pk);
            }else if(v < 0){
                AnnounceMessage pk("You lost "+std::to_string(v)+" rub");
                session->sendPacket(pk);
            }
        }
    }

    void User::addUsd(const int64_t& v){
        {
            std::lock_guard<std::mutex> lock(usdMutex);
            usd += v;
        }
        if(session != nullptr){
            if(v > 0){
                AnnounceMessage pk("You earned "+std::to_string(v)+" usd");
                session->sendPacket(pk);
            }else if(v < 0){
                AnnounceMessage pk("You lost "+std::to_string(v)+" usd");
                session->sendPacket(pk);
            }
        }
    }

}