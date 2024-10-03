#pragma once

#include <boost/asio.hpp>

namespace realtime_usd_market{

    class Packet;
    class ThreadPool;
    class Client{
        public:
            bool must_listen = true;
            short authorize_code = -1;
            bool authorized = false;
            bool disconnected = false;
            int64_t rub = 0;
            int64_t usd = 0;
            int64_t min_sell = -1;
            int64_t max_sell = -1;
            int64_t min_buy = -1;
            int64_t max_buy = -1;
            Client(bool listen = true);
            ~Client();
			Client(const Client&) = delete;
			Client& operator = (const Client&) = delete;

            void sendPacket(Packet& pk);
            void sendUserTransactionRequest(const short& type, const int64_t& r, const int64_t& u);
            void sendUserAuthorizeRequest(const std::string& login, const size_t& pswd);
            void markAsTest();
            void disconnect();

        private:
            ThreadPool* threadPool;
            boost::asio::io_service* io_service;
            boost::asio::ip::tcp::socket* socket;
            boost::asio::streambuf b;

            void read();

            void handleAnnounceMessage(char* data);
            void handleUserAuthorizeResponse(char* data);
            void handleCheckBalanceResponse(char* data);
    };
}