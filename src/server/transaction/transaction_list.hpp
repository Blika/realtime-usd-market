#pragma once
#include <mutex>
#include <vector>

namespace realtime_usd_market{

    class TransactionRequest;
    class TransactionList{
        public:

            std::mutex mtx;
            std::vector<TransactionRequest*> queue;

            TransactionList();
            ~TransactionList();
			TransactionList(const TransactionList&) = delete;
			TransactionList& operator = (const TransactionList&) = delete;
    };
}
