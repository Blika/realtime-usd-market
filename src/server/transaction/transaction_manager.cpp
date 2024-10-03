#include "transaction_manager.hpp"
#include "transaction_request.hpp"
#include "transaction_list.hpp"
#include "../user/user.hpp"
#include "../../utils/utils.hpp"

namespace realtime_usd_market{

    TransactionManager::TransactionManager(){

    }

    TransactionManager::~TransactionManager(){

        std::vector<int64_t> s;
        for(auto&[k,v]: sell_requests){
            s.push_back(k);
        }
        for(auto& v: s){
            std::lock_guard<std::mutex> lck(sell_requests[v]->mtx);
            delete sell_requests[v];
            sell_requests.erase(v);
        }
        std::vector<int64_t> b;
        for(auto&[k,v]: buy_requests){
            b.push_back(k);
        }
        for(auto& v: b){
            std::lock_guard<std::mutex> lck(buy_requests[v]->mtx);
            delete buy_requests[v];
            buy_requests.erase(v);
        }
        /*for(auto&[k,v]: sell_requests){
            delete v;
            sell_requests.erase(k);
        }
        for(auto&[k,v]: buy_requests){
            delete v;
            buy_requests.erase(k);
        }*/
    }

    int64_t TransactionManager::getBuyMin(){
        return buy_min;
    }
    int64_t TransactionManager::getBuyMax(){
        return buy_max;
    }
    int64_t TransactionManager::getSellMin(){
        return sell_min;
    }
    int64_t TransactionManager::getSellMax(){
        return sell_max;
    }

    // perform transaction as soon as we get them
    void TransactionManager::addRequest(User* user, const short& type, const int64_t& rub, const int64_t& usd){
        TransactionRequest* rq = new TransactionRequest(user, type, rub, usd);
        if(type == TRANSACTION_TYPE_SELL){
            int64_t _buy_max = buy_max;
            bool sameUser = false;
            sell:
            std::unique_lock<std::mutex> lock(sellMtx);
            if(_buy_max == -1){
                if(rq->getPrice() > sell_max){
                    sell_max = rq->getPrice();
                }
                if(sell_min == -1 || rq->getPrice() < sell_min){
                    sell_min = rq->getPrice();
                }
                if(sell_requests.contains(rq->getPrice())){  
                    {
                        std::lock_guard<std::mutex> lck(sell_requests[rq->getPrice()]->mtx);
                        sell_requests[rq->getPrice()]->queue.push_back(rq);
                    }
                }else{
                    sell_requests[rq->getPrice()] = new TransactionList;
                    {
                        std::lock_guard<std::mutex> lck(sell_requests[rq->getPrice()]->mtx);
                        sell_requests[rq->getPrice()]->queue.push_back(rq);
                    }
                }
                lock.unlock();
            }else{
                if(_buy_max >= rq->getPrice()){
                    lock.unlock();
                    {
                        std::lock_guard<std::mutex> lck(buy_requests[_buy_max]->mtx);
                        auto s = buy_requests[_buy_max]->queue.size();
                        size_t ind = -1;
                        for(auto i = 0; i < s; ++i){
                            if(buy_requests[_buy_max]->queue[i]->getUser() == rq->getUser()){
                                sameUser = true;
                                continue;
                            }else{
                                sameUser = false;
                                ind = i;
                                break;
                            }
                        }
                        if(ind == -1){
                            for(int64_t i = _buy_max-1; i >= -1; --i){
                                if(buy_requests.contains(i)){
                                    _buy_max = i;
                                    sameUser = false;
                                    break;
                                }
                            }
                            if(sameUser){
                                _buy_max = -1;
                                sameUser = false;
                            }
                            goto sell;
                        }
                        TransactionRequest* r = buy_requests[_buy_max]->queue[ind];
                        r->perform(rq);
                        if(r->fulfilled()){
                            delete r;
                            buy_requests[_buy_max]->queue.erase(buy_requests[_buy_max]->queue.begin()+ind);
                            if(buy_requests[_buy_max]->queue.empty()){
                                std::lock_guard<std::mutex> lock(buyMtx);
                                delete buy_requests[_buy_max];
                                buy_requests.erase(_buy_max);
                                if(!buy_requests.contains(buy_max)){
                                    int64_t _max = buy_max;
                                    buy_max = -1;
                                    for(int64_t i = _max - 1; i >= 0; --i){
                                        if(buy_requests.contains(i)){
                                            buy_max = i;
                                            break;
                                        }
                                    }
                                }
                                _buy_max = buy_max;
                                if(!buy_requests.contains(buy_min)){
                                    buy_min = -1;
                                    for(int64_t i = 0; i <= buy_max; ++i){
                                        if(buy_requests.contains(i)){
                                            buy_min = i;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        if(rq->fulfilled()){
                            delete rq;
                            return;
                        }
                        goto sell;
                    }
                }else{
                    if(rq->getPrice() > sell_max){
                        sell_max = rq->getPrice();
                    }
                    if(sell_min == -1 || rq->getPrice() < sell_min){
                        sell_min = rq->getPrice();
                    }
                    if(sell_requests.contains(rq->getPrice())){  
                        {
                            std::lock_guard<std::mutex> lck(sell_requests[rq->getPrice()]->mtx);
                            sell_requests[rq->getPrice()]->queue.push_back(rq);
                        }
                    }else{
                        sell_requests[rq->getPrice()] = new TransactionList;
                        {
                            std::lock_guard<std::mutex> lck(sell_requests[rq->getPrice()]->mtx);
                            sell_requests[rq->getPrice()]->queue.push_back(rq);
                        }
                    }
                    lock.unlock();
                }
            }
        }else{
            int64_t _sell_min = sell_min;
            bool sameUser = false;
            buy:
            std::unique_lock<std::mutex> lock(buyMtx);
            if(_sell_min == -1){
                if(rq->getPrice() > buy_max){
                    buy_max = rq->getPrice();
                }
                if(buy_min == -1 || rq->getPrice() < buy_min){
                    buy_min = rq->getPrice();
                }
                if(buy_requests.contains(rq->getPrice())){  
                    {
                        std::lock_guard<std::mutex> lck(buy_requests[rq->getPrice()]->mtx);
                        buy_requests[rq->getPrice()]->queue.push_back(rq);
                    }
                }else{
                    buy_requests[rq->getPrice()] = new TransactionList;
                    {
                        std::lock_guard<std::mutex> lck(buy_requests[rq->getPrice()]->mtx);
                        buy_requests[rq->getPrice()]->queue.push_back(rq);
                    }
                }
                lock.unlock();
            }else{
                if(_sell_min <= rq->getPrice()){
                    lock.unlock();
                    {
                        std::lock_guard<std::mutex> lck(sell_requests[_sell_min]->mtx);
                        auto s = sell_requests[_sell_min]->queue.size();
                        size_t ind = -1;
                        for(auto i = 0; i < s; ++i){
                            if(sell_requests[_sell_min]->queue[i]->getUser() == rq->getUser()){
                                sameUser = true;
                                continue;
                            }else{
                                sameUser = false;
                                ind = i;
                                break;
                            }
                        }
                        if(ind == -1){
                            for(int64_t i = _sell_min+1; i <= sell_max; ++i){
                                if(sell_requests.contains(i)){
                                    _sell_min = i;
                                    sameUser = false;
                                    break;
                                }
                            }
                            if(sameUser){
                                _sell_min = -1;
                                sameUser = false;
                            }
                            goto buy;
                        }
                        TransactionRequest* r = sell_requests[_sell_min]->queue[ind];
                        r->perform(rq);
                        if(r->fulfilled()){
                            delete r;
                            sell_requests[_sell_min]->queue.erase(sell_requests[_sell_min]->queue.begin()+ind);
                            if(sell_requests[_sell_min]->queue.empty()){
                                std::lock_guard<std::mutex> lock(buyMtx);
                                delete sell_requests[_sell_min];
                                sell_requests.erase(_sell_min);
                                if(!sell_requests.contains(sell_max)){
                                    int64_t _max = sell_max;
                                    sell_max = -1;
                                    for(int64_t i = _max - 1; i >= 0; --i){
                                        if(sell_requests.contains(i)){
                                            sell_max = i;
                                            break;
                                        }
                                    }
                                }
                                if(!sell_requests.contains(sell_min)){
                                    sell_min = -1;
                                    for(int64_t i = 0; i <= sell_max; ++i){
                                        if(sell_requests.contains(i)){
                                            sell_min = i;
                                            break;
                                        }
                                    }
                                }
                                _sell_min = sell_min;
                            }
                        }
                        if(rq->fulfilled()){
                            delete rq;
                            return;
                        }
                        goto buy;
                    }
                }else{
                    if(rq->getPrice() > buy_max){
                        buy_max = rq->getPrice();
                    }
                    if(buy_min == -1 || rq->getPrice() < buy_min){
                        buy_min = rq->getPrice();
                    }
                    if(buy_requests.contains(rq->getPrice())){  
                        {
                            std::lock_guard<std::mutex> lck(buy_requests[rq->getPrice()]->mtx);
                            buy_requests[rq->getPrice()]->queue.push_back(rq);
                        }
                    }else{
                        buy_requests[rq->getPrice()] = new TransactionList;
                        {
                            std::lock_guard<std::mutex> lck(buy_requests[rq->getPrice()]->mtx);
                            buy_requests[rq->getPrice()]->queue.push_back(rq);
                        }
                    }
                    lock.unlock();
                }
            }
        }
    }
}