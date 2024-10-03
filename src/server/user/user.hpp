#pragma once

#include <cstdint>
#include <mutex>
#include <string>

namespace realtime_usd_market{
    class Session;
    class User{
        public:
            bool test = false;
            User(const std::string& login,const size_t& pswd);
            ~User();
            User(const User&) = delete;
            User& operator = (const User&) = delete;

            std::string getLogin();
            bool comparePassword(const size_t& password);
            bool hasSession();
            void setSession(Session* s);
            int64_t getRub();
            int64_t getUsd();
            void addRub(const int64_t& v);
            void addUsd(const int64_t& v);

        private:
            std::string login;
            size_t pswd;
            std::mutex rubMutex;
            std::mutex usdMutex;
            int64_t rub = 0;
            int64_t usd = 0;
            Session* session = nullptr;
    };
}