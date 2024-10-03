#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>

namespace realtime_usd_market{

    const short TRANSACTION_TYPE_SELL = 0;
    const short TRANSACTION_TYPE_BUY = 1;

    class User;
    class TransactionRequest{
        public:
            std::atomic<bool> locked = false;
            TransactionRequest(User* u, const short& t, const int64_t& rub, const int64_t& usd);
            ~TransactionRequest();
			TransactionRequest(const TransactionRequest&) = delete;
			TransactionRequest& operator = (const TransactionRequest&) = delete;

            bool fulfilled();
            void perform(TransactionRequest* r);
            short getType();
            User* getUser();
            int64_t getPrice();
            int64_t getUsd();
            void addRub(const int64_t& v);
            void addUsd(const int64_t& v);

        private:
            std::mutex mtx;
            short type;
            User* user = nullptr;
            int64_t usd;
            int64_t price;
    };
}