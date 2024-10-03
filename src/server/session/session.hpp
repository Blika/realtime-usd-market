#pragma once

#include <stdint.h>
#include <boost/asio.hpp>

namespace realtime_usd_market{
    class User;
    class Packet;
    class Session{
        public:
            Session(boost::asio::io_service* io_service);
            ~Session();
			Session(const Session&) = delete;
			Session& operator = (const Session&) = delete;

            void close();
            std::string getAddress();
            void setAddress();
            boost::asio::ip::tcp::socket* getSocket();
            void read();
            void sendPacket(Packet& pk);
            void setUser(User* user);

        private:
            bool closed = false;
            boost::asio::ip::tcp::socket* socket;
            boost::asio::streambuf streambuf;
            std::string address;
            User* user = nullptr;

            void processRead(const boost::system::error_code& error, size_t bytes_transferred);
            void processWrite(const boost::system::error_code& error);

            void handleAnnounceMessage(char* data);
            void handleUserAuthorize(char* data);
            void handleCheckBalanceRequest();
            void handleTestPacket();
            void handleUserTransactionRequest(char* data);
    };
}