#include "client.hpp"
#include "../../utils/json.hpp"
#include "../../utils/utils.hpp"
#include "../../threadpool/threadpool.hpp"
#include "../../packets/packets.hpp"
#include <fstream>
#include <stdexcept>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

namespace realtime_usd_market{

    Client::Client(bool listen){
        must_listen = listen;
        nlohmann::json properties;
        uint16_t port;
	    try{
            std::ifstream mf("prefs/settings.json");
            properties = nlohmann::json::parse(mf);
            mf.close();
            port = properties["port"].get<uint16_t>();
        }catch(const std::exception& e){
            throw std::runtime_error("Failed to read settings file");
	    }
        io_service = new boost::asio::io_service();
        boost::asio::ip::tcp::resolver resolver(*io_service);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), "127.0.0.1", std::to_string(port));
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
        socket = new boost::asio::ip::tcp::socket(*io_service);
        socket->connect(*iterator);
        send_log("Connected to server");
        threadPool = new ThreadPool();
        if(must_listen){
            threadPool->assignNewTask(std::bind(&Client::read, this),-1);
        }
    }

    Client::~Client(){
        if(!disconnected) disconnect();
    }
    
    void Client::disconnect(){
        if(disconnected) return;
        disconnected = true;
        threadPool->wait();
        socket->cancel();
        socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket->close();
        io_service->stop();
        delete socket;
        delete io_service;
        if(must_listen){
            // hack
            // break free from std::cin lock
            throw std::runtime_error("Disconnected from server");
        }
    }

    void Client::sendPacket(Packet& pk){
        std::stringstream ss;
        pk.encode(ss);
        std::string d = ss.str();
        boost::system::error_code error;
        boost::asio::write(*socket,boost::asio::buffer(d, d.size()),error);
        if(error){
            send_error(error.message());
            disconnect();
        }
    }

    void Client::sendUserTransactionRequest(const short& type, const int64_t& r, const int64_t& u){
        UserTransactionRequest pk(r,u,type);
        sendPacket(pk);
    }

    void Client::sendUserAuthorizeRequest(const std::string& login, const size_t& pswd){
        UserAuthorize pk(login,pswd);
        sendPacket(pk);
    }

    void Client::markAsTest(){
        TestPacket pk;
        sendPacket(pk);
    }

    void Client::read(){
        while(true){
            if(disconnected) break;
            boost::system::error_code error;
            boost::asio::streambuf streambuf;
            size_t bytes_transferred = boost::asio::read_until(*socket, streambuf,'|', error);
            if(error){
                send_error(error.message());
                disconnect();
                break;
            }
            // sometimes (too much incoming data) we get multiple packets in one stream
            std::string str((std::istreambuf_iterator<char>(&streambuf)), std::istreambuf_iterator<char>());
            std::vector<std::string> pks;
            boost::split(pks, str, boost::is_any_of("|"));
            for(auto& s: pks){
                if(s.empty()) continue;
                char* data = (char*) s.c_str();
                unsigned char id = data[0];
                data[0] = '\n';
                switch(id){
                    case ANNOUNCE_MESSAGE:
                        handleAnnounceMessage(data);
                        break;
                    case USER_AUTHORIZE_RESPONSE:
                        handleUserAuthorizeResponse(data);
                        break;
                    case CHECK_BALANCE_RESPONSE:
                        handleCheckBalanceResponse(data);
                        break;
                    default:
                        send_error("Unknown packet ",id," from server");
                        break;
                }
            }
        }
    }

    void Client::handleAnnounceMessage(char* data){
        AnnounceMessage pk = AnnounceMessage::decode(data);
        send_log("Received message ",pk.message);
    }

    void Client::handleUserAuthorizeResponse(char* data){
        UserAuthorizeResponse pk = UserAuthorizeResponse::decode(data);
        if(pk.code == AUTHORIZE_SUCCESS){
            send_log("Successfully authorized");
            authorized = true;
            authorize_code = pk.code;
        }else if(pk.code == AUTHORIZE_WRONG_PASSWORD){
            send_log("Wrong password");
            authorize_code = pk.code;
        }else if(pk.code == AUTHORIZE_ALREADY_AUTHORIZED){
            send_log("User already authorized");
            authorize_code = pk.code;
        }
    }

    void Client::handleCheckBalanceResponse(char* data){
        CheckBalanceResponse pk = CheckBalanceResponse::decode(data);
        rub = pk.rub;
        usd = pk.usd;
        min_sell = pk.min_sell;
        max_sell = pk.max_sell;
        min_buy = pk.min_buy;
        max_buy = pk.max_buy;
    }

}