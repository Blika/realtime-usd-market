#include "session.hpp"
#include "../server/server.hpp"
#include "../user/user.hpp"
#include "../transaction/transaction_manager.hpp"
#include "../../packets/packets.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/json.hpp"
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

namespace realtime_usd_market{

    Session::Session(boost::asio::io_service* io_service){
        socket = new boost::asio::ip::tcp::socket(*io_service);
    }

    Session::~Session(){
        if(!closed) close();
    }

    boost::asio::ip::tcp::socket* Session::getSocket(){
        return socket;
    }

    std::string Session::getAddress(){
        return address;
    }

    void Session::setAddress(){
        address = socket->remote_endpoint().address().to_string() + ":" +std::to_string(socket->remote_endpoint().port());
    }

    void Session::setUser(User* u){
        user = u;
    }

    void Session::close(){
        if(closed) return;
        send_log("Closed connection: ",getAddress());
        closed = true;
        if(user != nullptr){
            user->setSession(nullptr);
            user = nullptr;
        }
        boost::system::error_code error;
        socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both,error);
        error.clear();
        socket->close(error);
        delete socket;
    }

    void Session::sendPacket(Packet& pk){
        std::stringstream ss;
        pk.encode(ss);
        std::string d = ss.str();
        boost::asio::async_write(*socket,boost::asio::buffer(d, d.size()),boost::bind(&Session::processWrite, this, boost::asio::placeholders::error));
    }
    
    void Session::processWrite(const boost::system::error_code& error){
    }

    void Session::read(){
        boost::asio::async_read_until(*socket, streambuf, '|', boost::bind(&Session::processRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void Session::processRead(const boost::system::error_code& error, size_t bytes_transferred){
        if(Server::getInstance()->stopped) return;
        if(error){
            Server::getInstance()->closeSession(this);
            return;
        }
        // sometimes (too much incoming data) we get multiple packets in one stream
        std::string str((std::istreambuf_iterator<char>(&streambuf)), std::istreambuf_iterator<char>());
        std::vector<std::string> pks;
        boost::split(pks, str, boost::is_any_of("|"));
        for(auto& s: pks){
            if(s.empty()) continue;
            char* data = (char*) s.c_str();
            unsigned char id = (unsigned char) data[0];
            data[0] = '\n';
            switch(id){
                case ANNOUNCE_MESSAGE:
                    handleAnnounceMessage(data);
                    break;
                case USER_AUTHORIZE:
                    handleUserAuthorize(data);
                    break;
                case CHECK_BALANCE_REQUEST:
                    if(user == nullptr) break;
                    handleCheckBalanceRequest();
                    break;
                case USER_TRANSACTION_REQUEST:
                    if(user == nullptr) break;
                    handleUserTransactionRequest(data);
                    break;
                case TEST_PACKET:
                    if(user == nullptr) break;
                    handleTestPacket();
                    break;
                default:
                    send_error("Unknown packet ",id," from ",getAddress());
                    break;
            }
        }
        read();
    }

    void Session::handleAnnounceMessage(char* data){
        AnnounceMessage pk = AnnounceMessage::decode(data);
        send_log("Received message ",pk.message);
    }

    void Session::handleUserAuthorize(char* data){
        UserAuthorize pk = UserAuthorize::decode(data);
        if(pk.login == ""){
            UserAuthorizeResponse pk1(AUTHORIZE_ALREADY_AUTHORIZED);
            sendPacket(pk1);
            return;
        }
        User* user = Server::getInstance()->getUser(pk.login);
        if(user != nullptr){
            if(user->hasSession()){
                UserAuthorizeResponse pk1(AUTHORIZE_ALREADY_AUTHORIZED);
                sendPacket(pk1);
                return;
            }else if(user->comparePassword(pk.pswd)){
                user->setSession(this);
                setUser(user);
                send_log("User ",pk.login," has been authorized");
                UserAuthorizeResponse pk1(AUTHORIZE_SUCCESS);
                sendPacket(pk1);
                return;
            }else{
                UserAuthorizeResponse pk1(AUTHORIZE_WRONG_PASSWORD);
                sendPacket(pk1);
                return;
            }
        }
        Server::getInstance()->addUser(pk.login, pk.pswd);
        user = Server::getInstance()->getUser(pk.login);
        user->setSession(this);
        setUser(user);
        send_log("User ",pk.login," has been authorized");
        UserAuthorizeResponse pk1(AUTHORIZE_SUCCESS);
        sendPacket(pk1);
    }

    void Session::handleCheckBalanceRequest(){
        CheckBalanceResponse pk(user->getRub(), user->getUsd(), Server::getInstance()->getTransactionManager()->getSellMin(), Server::getInstance()->getTransactionManager()->getSellMax(), Server::getInstance()->getTransactionManager()->getBuyMin(), Server::getInstance()->getTransactionManager()->getBuyMax());
        sendPacket(pk);
    }

    void Session::handleTestPacket(){
        user->test = true;
    }

    void Session::handleUserTransactionRequest(char* data){
        UserTransactionRequest pk = UserTransactionRequest::decode(data);
        if(pk.usd == 0) return;
        send_log("New transaction request from ",user->getLogin()," (",(pk.type == 0 ? "SELL":"BUY")," USD: ",pk.usd,"; RUB: ",pk.rub,")");
        Server::getInstance()->getTransactionManager()->addRequest(user, pk.type, pk.rub, pk.usd);
    }
}