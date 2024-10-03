#include "server.hpp"
#include "../command/command_executor.hpp"
#include "../session/session.hpp"
#include "../user/user.hpp"
#include "../transaction/transaction_manager.hpp"
#include "../../threadpool/threadpool.hpp"
#include "../../utils/json.hpp"
#include "../../utils/utils.hpp"
#include <fstream>
#include <stdexcept>
#include <boost/bind.hpp>

namespace realtime_usd_market{

	Server* Server::instance = nullptr;

    Server::Server(){
        instance = this;
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
        acceptor = new boost::asio::ip::tcp::acceptor(*io_service,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
        send_log("Listening ",port);
        threadPool = new ThreadPool();
        threadPool->assignNewTask(std::bind(&Server::input, this),-1);
        transactionManager = new TransactionManager();
        listen();
        runIoService();
    }

    Server::~Server(){
        if(!stopped) shutdown();
    }

    Server* Server::getInstance(){
        return Server::instance;
    }

    TransactionManager* Server::getTransactionManager(){
        return transactionManager;
    }

    void Server::shutdown(){
        if(stopped) return;
        stopped = true;
        threadPool->wait();
        io_service->stop();
        delete transactionManager;
        std::vector<Session*> s;
        for(auto&[k,v]: sessions){
            s.push_back(v);
        }
        for(auto& v: s){
            closeSession(v);
        }
        std::vector<User*> u;
        for(auto&[k,v]: users){
            u.push_back(v);
        }
        for(auto& v: u){
            removeUser(v->getLogin());
        }
        delete acceptor;
        delete session;
        delete io_service;
        finally_shutdown = true;
    }

    void Server::broadcastPacket(Packet* pk){
        for(auto&[k,v]: sessions){
            v->sendPacket(*pk);
        }
    }

    User* Server::getUser(const std::string& login){
        if(!hasUser(login)) return nullptr;
        std::lock_guard<std::mutex> lock(usersMutex);
        return users[login];
    }

    bool Server::hasUser(const std::string& login){
        std::lock_guard<std::mutex> lock(usersMutex);
        return users.contains(login);
    }

    void Server::addUser(const std::string& login, const size_t& pswd){
        if(hasUser(login)) return;
        std::lock_guard<std::mutex> lock(usersMutex);
        users[login] = new User(login,pswd);
    }

    void Server::removeUser(const std::string& login){
        if(!hasUser(login)) return;
        std::lock_guard<std::mutex> lock(usersMutex);
        delete users[login];
        users.erase(login);
    }

    void Server::input(){
        while(true){
            if(stopped) break;
            std::string in;
            std::getline(std::cin, in);
            CommandExecutor::execute(in);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    void Server::listen(){
        session = new Session(io_service);
        acceptor->async_accept(*session->getSocket(), boost::bind(&Server::handleAccept, this, session, boost::asio::placeholders::error));
    }

    void Server::handleAccept(Session* session, const boost::system::error_code& error){
        session->setAddress();
        if(!error){
            if(hasSession(session->getAddress())){
                send_log("Ignored incoming connection ",session->getAddress()," (connection already open)");
                session = nullptr;
            }else{
                createSession(session);
                session->read();
            }
        }else{
            session = nullptr;
        }
        listen();
    }

    bool Server::hasSession(const std::string& session){
        std::lock_guard<std::mutex> lock(sessionsMutex);
        return sessions.contains(session);
    }

    void Server::createSession(Session* session){
        if(hasSession(session->getAddress())) return;
        send_log("Accepted new connection: ",session->getAddress());
        std::lock_guard<std::mutex> lock(sessionsMutex);
        sessions[session->getAddress()] = session;
    }

    void Server::closeSession(const std::string& session){
        if(!hasSession(session)) return;
        sessions[session]->close();
        std::lock_guard<std::mutex> lock(sessionsMutex);
        delete sessions[session];
        sessions.erase(session);
    }
    void Server::closeSession(Session* session){
        std::string address = session->getAddress();
        if(!hasSession(address)) return;
        sessions[address]->close();
        std::lock_guard<std::mutex> lock(sessionsMutex);
        delete sessions[address];
        sessions.erase(address);
    }

    void Server::runIoService(){
        io_service->run();
        while(!finally_shutdown){

        }
    }
}