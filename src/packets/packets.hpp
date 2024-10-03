#pragma once

#include "packet_ids.hpp"
#include "../utils/json.hpp"
#include "../utils/utils.hpp"
#include <stdint.h>

namespace realtime_usd_market{

    const short AUTHORIZE_SUCCESS = 0;
    const short AUTHORIZE_WRONG_PASSWORD = 1;
    const short AUTHORIZE_ALREADY_AUTHORIZED = 2;

    class Packet{
        public:
            unsigned char typeId = VOID_PACKET;
            Packet(){
            }
            ~Packet(){
            }
            virtual void encode(std::stringstream& stream){
                stream << std::hex << typeId;
                stream << '|';
            }
            static Packet decode(char* data){
                return Packet();
            }
    };
    class AnnounceMessage: public Packet{
        public:
            unsigned char typeId = ANNOUNCE_MESSAGE;
            std::string message;
            AnnounceMessage(const std::string& m){
                message = m;
            }
            ~AnnounceMessage(){
            }
            void encode(std::stringstream& stream){
                stream << std::hex << typeId;
                nlohmann::json json_data;
                json_data[0] = message;
                stream << json_data.dump();
                stream << '|';
            }
            static AnnounceMessage decode(char* data){
                nlohmann::json recv;
                try{
                    recv = nlohmann::json::parse(data);
                    std::string message = recv[0].get<std::string>();
                    return AnnounceMessage(message);
                }catch(const std::exception& e){
                    send_error("Failed to decode AnnounceMessage packet");
                    return AnnounceMessage("");
                }
            }
    };
    class UserAuthorize: public Packet{
        public:
            unsigned char typeId = USER_AUTHORIZE;
            std::string login;
            size_t pswd;
            UserAuthorize(const std::string& l, const size_t& p){
                login = l;
                pswd = p;
            }
            ~UserAuthorize(){
            }
            void encode(std::stringstream& stream){
                stream << std::hex << typeId;
                nlohmann::json json_data;
                json_data[0] = login;
                json_data[1] = pswd;
                stream << json_data.dump();
                stream << '|';
            }
            static UserAuthorize decode(char* data){
                nlohmann::json recv;
                try{
                    recv = nlohmann::json::parse(data);
                    std::string login = recv[0].get<std::string>();
                    size_t pswd = recv[1].get<size_t>();
                    return UserAuthorize(login, pswd);
                }catch(const std::exception& e){
                    send_error("Failed to decode UserAuthorize packet");
                    return UserAuthorize("", 0);
                }
            }
    };
    class UserAuthorizeResponse: public Packet{
        public:
            unsigned char typeId = USER_AUTHORIZE_RESPONSE;
            short code;
            UserAuthorizeResponse(const short& c){
                code = c;
            }
            ~UserAuthorizeResponse(){
            }
            void encode(std::stringstream& stream){
                stream << std::hex << typeId;
                nlohmann::json json_data;
                json_data[0] = code;
                stream << json_data.dump();
                stream << '|';
            }
            static UserAuthorizeResponse decode(char* data){
                nlohmann::json recv;
                try{
                    recv = nlohmann::json::parse(data);
                    short code = recv[0].get<short>();
                    return UserAuthorizeResponse(code);
                }catch(const std::exception& e){
                    send_error("Failed to decode UserAuthorizeResponse packet");
                    return UserAuthorizeResponse(0);
                }
            }
    };
    class CheckBalanceRequest: public Packet{
        public:
            unsigned char typeId = CHECK_BALANCE_REQUEST;
            CheckBalanceRequest(){
            }
            ~CheckBalanceRequest(){
            }
            void encode(std::stringstream& stream){
                stream << std::hex << typeId;
                stream << '|';
            }
            static CheckBalanceRequest decode(char* data){
                return CheckBalanceRequest();
            }
    };
    class CheckBalanceResponse: public Packet{
        public:
            unsigned char typeId = CHECK_BALANCE_RESPONSE;
            int64_t rub;
            int64_t usd;
            int64_t min_sell;
            int64_t max_sell;
            int64_t min_buy;
            int64_t max_buy;
            CheckBalanceResponse(const int64_t& r, const int64_t& u, const int64_t& mins, const int64_t& maxs, const int64_t& minb, const int64_t& maxb){
                rub = r;
                usd = u;
                min_sell = mins;
                max_sell = maxs;
                min_buy = minb;
                max_buy = maxb;
            }
            ~CheckBalanceResponse(){
            }
            void encode(std::stringstream& stream){
                stream << std::hex << typeId;
                nlohmann::json json_data;
                json_data[0] = rub;
                json_data[1] = usd;
                json_data[2] = min_sell;
                json_data[3] = max_sell;
                json_data[4] = min_buy;
                json_data[5] = max_buy;
                stream << json_data.dump();
                stream << '|';
            }
            static CheckBalanceResponse decode(char* data){
                nlohmann::json recv;
                try{
                    recv = nlohmann::json::parse(data);
                    int64_t rub = recv[0].get<int64_t>();
                    int64_t usd = recv[1].get<int64_t>();
                    int64_t min_sell = recv[2].get<int64_t>();
                    int64_t max_sell = recv[3].get<int64_t>();
                    int64_t min_buy = recv[4].get<int64_t>();
                    int64_t max_buy = recv[5].get<int64_t>();
                    return CheckBalanceResponse(rub,usd,min_sell,max_sell,min_buy,max_buy);
                }catch(const std::exception& e){
                    send_error("Failed to decode CheckBalanceResponse packet");
                    return CheckBalanceResponse(0,0,0,0,0,0);
                }
            }
    };
    class UserTransactionRequest: public Packet{
        public:
            unsigned char typeId = USER_TRANSACTION_REQUEST;
            int64_t rub;
            int64_t usd;
            short type;
            UserTransactionRequest(const int64_t& r, const int64_t& u, const short& t){
                rub = r;
                usd = u;
                type = t;
            }
            ~UserTransactionRequest(){
            }
            void encode(std::stringstream& stream){
                stream << std::hex << typeId;
                nlohmann::json json_data;
                json_data[0] = rub;
                json_data[1] = usd;
                json_data[2] = type;
                stream << json_data.dump();
                stream << '|';
            }
            static UserTransactionRequest decode(char* data){
                nlohmann::json recv;
                try{
                    recv = nlohmann::json::parse(data);
                    int64_t rub = recv[0].get<int64_t>();
                    int64_t usd = recv[1].get<int64_t>();
                    int64_t type = recv[2].get<short>();
                    return UserTransactionRequest(rub,usd,type);
                }catch(const std::exception& e){
                    send_error("Failed to decode UserTransactionRequest packet");
                    return UserTransactionRequest(0,0,0);
                }
            }
    };
    class TestPacket: public Packet{
        public:
            unsigned char typeId = TEST_PACKET;
            TestPacket(){
            }
            ~TestPacket(){
            }
            void encode(std::stringstream& stream){
                stream << std::hex << typeId;
                stream << '|';
            }
            static TestPacket decode(char* data){
                return TestPacket();
            }
    };
}