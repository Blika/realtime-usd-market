#pragma once

#include <boost/asio.hpp>
#include <unordered_map>
#include <mutex>

namespace realtime_usd_market{
    class Session;
    class User;
    class Packet;
    class ThreadPool;
    class TransactionManager;
    class Server{
        public:
            bool stopped = false;
            Server();
            ~Server();
			Server(const Server&) = delete;
			Server& operator = (const Server&) = delete;

            static Server* getInstance();
            TransactionManager* getTransactionManager();
            void shutdown();
            void closeSession(const std::string& session);
            void closeSession(Session* session);
            void broadcastPacket(Packet* pk);
            User* getUser(const std::string& login);
            void addUser(const std::string& login, const size_t& pswd);
            bool hasUser(const std::string& login);
            void removeUser(const std::string& login);

        private:
            bool finally_shutdown = false;
            static Server* instance;
            ThreadPool* threadPool;
            TransactionManager* transactionManager;
            boost::asio::io_service* io_service;
            boost::asio::ip::tcp::acceptor* acceptor;
            std::unordered_map<std::string, Session*> sessions;
            std::unordered_map<std::string, User*> users;
            Session* session;
            std::mutex usersMutex;
            std::mutex sessionsMutex;

            void input();
            bool hasSession(const std::string& address);
            void createSession(Session* session);
            void listen();
            void handleAccept(Session* session, const boost::system::error_code& error);
            void runIoService();
    };
}