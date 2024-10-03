#pragma once

#include <unordered_map>
#include <cstdint>
#include <queue>
#include <mutex>

namespace realtime_usd_market{

    class User;
    class TransactionRequest;
    class TransactionList;
    class TransactionManager{
        public:
            TransactionManager();
            ~TransactionManager();
			TransactionManager(const TransactionManager&) = delete;
			TransactionManager& operator = (const TransactionManager&) = delete;

            void addRequest(User* user, const short& type, const int64_t& rub, const int64_t& usd);
            int64_t getBuyMin();
            int64_t getBuyMax();
            int64_t getSellMin();
            int64_t getSellMax();

        private:
            // <rub, TransactionList*>
            std::unordered_map<int64_t, TransactionList*> buy_requests;
            // <rub, TransactionList*>
            std::unordered_map<int64_t, TransactionList*> sell_requests;
            int64_t buy_min = -1;
            int64_t buy_max = -1;
            int64_t sell_min = -1;
            int64_t sell_max = -1;
            std::mutex buyMtx;
            std::mutex sellMtx;
    };
}