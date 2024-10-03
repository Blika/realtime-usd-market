#include "transaction_request.hpp"
#include "../user/user.hpp"
#include "../server/server.hpp"
#include "../../utils/utils.hpp"
#include "../../packets/packets.hpp"

namespace realtime_usd_market{

    TransactionRequest::TransactionRequest(User* u, const short& t, const int64_t& r, const int64_t& ud){
        user = u;
        type = t;
        price = r;
        usd = ud;
    }

    TransactionRequest::~TransactionRequest(){
        if(fulfilled()){
            send_log(user->getLogin(),"'s request has been fulfilled. Transaction request closed.");
        }else{
            send_log(user->getLogin(),"'s request has not been fulfilled. Transaction request closed.");
        }
    }

    bool TransactionRequest::fulfilled(){
        return usd == 0;
    }

    // we only care about usd part being fulfilled
    // users can have negative rub balance
    void TransactionRequest::perform(TransactionRequest* r){
        if(type == TRANSACTION_TYPE_SELL && r->getType() == TRANSACTION_TYPE_BUY){
            int64_t u1 = r->getUsd();
            int64_t u2 = getUsd();
            if(u1 <= u2){
                addUsd(-u1);
                addRub(r->getPrice() * u1);
                r->addUsd(-u1);
                r->addRub(-r->getPrice() * u1);
            }else{
                addUsd(-u2);
                addRub(r->getPrice() * u2);
                r->addUsd(-u2);
                r->addRub(-r->getPrice() * u2);
            }
        }else if(type == TRANSACTION_TYPE_BUY && r->getType() == TRANSACTION_TYPE_SELL){
            int64_t u1 = r->getUsd();
            int64_t u2 = getUsd();
            if(u1 <= u2){
                addUsd(-u1);
                addRub(-r->getPrice() * u1);
                r->addUsd(-u1);
                r->addRub(r->getPrice() * u1);
            }else{
                addUsd(-u2);
                addRub(-r->getPrice() * u2);
                r->addUsd(-u2);
                r->addRub(r->getPrice() * u2);
            }
        }
        if(user->test){
            std::string msg = user->getLogin()+" has "+std::to_string(user->getRub())+" RUB; "+std::to_string(user->getUsd())+" USD";
            AnnounceMessage pk(msg);
            Server::getInstance()->broadcastPacket(&pk);
        }
        if(r->getUser()->test){
            std::string msg = r->getUser()->getLogin()+" has "+std::to_string(r->getUser()->getRub())+" RUB; "+std::to_string(r->getUser()->getUsd())+" USD";
            AnnounceMessage pk(msg);
            Server::getInstance()->broadcastPacket(&pk);
        }
    }

    short TransactionRequest::getType(){
        return type;
    }

    User* TransactionRequest::getUser(){
        return user;
    }
    
    int64_t TransactionRequest::getPrice(){
        return price;
    }

    int64_t TransactionRequest::getUsd(){
        return usd;
    }

    void TransactionRequest::addRub(const int64_t& v){
        user->addRub(v);
    }

    void TransactionRequest::addUsd(const int64_t& v){
        usd += v;
        if(type == TRANSACTION_TYPE_BUY){
            user->addUsd(-v);
        }else{
            user->addUsd(v);
        }
    }


}