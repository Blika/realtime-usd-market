#include "transaction_list.hpp"
#include "transaction_request.hpp"

namespace realtime_usd_market{

    TransactionList::TransactionList(){

    }

    TransactionList::~TransactionList(){
        auto s = queue.size();
        for(auto i = 0; i < s; ++i){
            auto v = queue.front();
            delete v;
            queue.erase(queue.begin()+i);
        }
    }

}