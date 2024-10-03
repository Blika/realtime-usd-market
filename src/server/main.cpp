#include "server/server.hpp"
#include <iostream>

realtime_usd_market::Server* server = nullptr;

int main(){
	try{
        server = new realtime_usd_market::Server();
	}catch(const std::exception& e){
		std::cerr << e.what() << '\n';
	}
	std::exit(0);
    return 0;
}