#include "client/client.hpp"
#include "../packets/packets.hpp"
#include <iostream>
#include <functional>
#include <stdexcept>

using namespace realtime_usd_market;

int main(){
    Client* client;
	try{
        client = new Client();
        reg:
        {
            std::string login,password;
            std::cout << "Login: ";
            std::cin >> login;
            std::cout << "Passowrd: ";
            std::cin >> password;
            // unsafe hash
            size_t hash_pswd = std::hash<std::string>{}(password);
            client->sendUserAuthorizeRequest(login,hash_pswd);
        }
        while(!client->authorized){
            if(client->authorize_code > 0){
                client->authorize_code = -1;
                goto reg;
            }
        }
        short stage = 0;
        while(!client->disconnected){
            {
                CheckBalanceRequest pk;
                client->sendPacket(pk);
            }
            switch(stage){
                case 0:
                    {
                        std::cout << "Menu:\n"
                                        "1) Tests\n"
                                        "2) Balance\n"
                                        "3) Buy/sell USD\n"
                                        "4) Exit\n"
                        << std::flush;
                        std::string in;
                        std::cin >> in;
                        if(client->disconnected) break;
                        int opt = std::atoi(in.c_str());
                        switch(opt){
                            case 1:
                                stage = 1;
                                break;
                            case 2:
                                std::cout << "Your RUB: " << client->rub << "\n"
                                            "Your USD: " << client->usd << "\n"
                                            "Current min USD sell price (per unit): " << client->min_sell << " RUB\n"
                                            "Current max USD sell price (per unit): " << client->max_sell << " RUB\n"
                                            "Current min USD buy price (per unit): " << client->min_buy << " RUB\n"
                                            "Current max USD buy price (per unit): " << client->max_buy << " RUB\n"
                                            << std::flush;
                                break;
                            case 3:
                                stage = 2;
                                break;
                            case 4:
                                client->disconnect();
                                break;
                            default:
                                std::cout << "Unknown option" << '\n';
                                break;
                        }
                        break;
                    }
                case 1:
                    {
                        std::cout << "Tests:\n"
                                        "1) Test message to server\n"
                                        "2) Test 1\n"
                                        "3) Test 2\n"
                                        "4) Back\n"
                        << std::flush;
                        std::string in;
                        std::cin >> in;
                        if(client->disconnected) break;
                        int opt = std::atoi(in.c_str());
                        switch(opt){
                            case 1:
                                {
                                    AnnounceMessage pk("sosiska request");
                                    client->sendPacket(pk);
                                    std::cout << "Message sent" << '\n';
                                    break;
                                }
                            case 2:
                                {
                                    try{
                                        Client cl1(false);
                                        Client cl2(false);
                                        Client cl3(false);
                                        cl1.sendUserAuthorizeRequest("Test1User1",439548420);
                                        cl2.sendUserAuthorizeRequest("Test1User2",439548420);
                                        cl3.sendUserAuthorizeRequest("Test1User3",439548420);
                                        cl1.markAsTest();
                                        cl2.markAsTest();
                                        cl3.markAsTest();
                                        cl1.sendUserTransactionRequest(1, 62, 10);
                                        cl2.sendUserTransactionRequest(1, 63, 20);
                                        cl3.sendUserTransactionRequest(0, 61, 50);
                                    }catch(const std::exception& e){
		                                std::cerr << e.what() << '\n';
	                                }
                                    break;
                                }
                            case 3:
                                {
                                    // results may vary in this test
                                    // since it is a "real-time" project, all transaction are performed as soon as server gets them
                                    // packets may come in random order, thus sometimes there just isn't a better offer
                                    // depending on that order, the transaction may occur between User4 and User1 or User4 and User2
                                    try{
                                        Client cl1(false);
                                        Client cl2(false);
                                        Client cl3(false);
                                        Client cl4(false);
                                        Client cl5(false);
                                        cl1.sendUserAuthorizeRequest("Test2User1",439548420);
                                        cl2.sendUserAuthorizeRequest("Test2User2",439548420);
                                        cl3.sendUserAuthorizeRequest("Test2User3",439548420);
                                        cl4.sendUserAuthorizeRequest("Test2User4",439548420);
                                        cl5.sendUserAuthorizeRequest("Test2User5",439548420);
                                        cl1.markAsTest();
                                        cl2.markAsTest();
                                        cl3.markAsTest();
                                        cl4.markAsTest();
                                        cl5.markAsTest();
                                        cl1.sendUserTransactionRequest(0, 50, 30);
                                        cl2.sendUserTransactionRequest(0, 48, 20);
                                        cl3.sendUserTransactionRequest(1, 45, 10);
                                        cl4.sendUserTransactionRequest(1, 55, 20);
                                        cl5.sendUserTransactionRequest(0, 60, 50);
                                    }catch(const std::exception& e){
		                                std::cerr << e.what() << '\n';
	                                }
                                    break;
                                }
                            case 4:
                                stage = 0;
                                break;
                            default:
                                std::cout << "Unknown option" << '\n';
                                break;
                        }
                        break;
                    }
                case 2:
                    {
                        std::cout << "Current min USD sell price (per unit): " << client->min_sell << " RUB\n"
                                    "Current max USD sell price (per unit): " << client->max_sell << " RUB\n"
                                    "Current min USD buy price (per unit): " << client->min_buy << " RUB\n"
                                    "Current max USD buy price (per unit): " << client->max_buy << " RUB\n"
                                    << std::flush;
                        std::cout << "Buy/sell:\n"
                                    "1) Buy\n"
                                    "2) Sell\n"
                                    "3) Back\n"
                        << std::flush;
                        std::string in;
                        std::cin >> in;
                        if(client->disconnected) break;
                        int opt = std::atoi(in.c_str());
                        switch(opt){
                            case 1:
                                {
                                    int64_t usd,rub;
                                    std::cout << "How much USD you want to buy: ";
                                    std::string in1;
                                    std::cin >> in1;
                                    usd = std::atoi(in1.c_str());
                                    if(usd <= 0){
                                        std::cout << "Amount must be greater than zero\n";
                                        break;
                                    }
                                    std::cout << "How much RUB per unit you want to pay: ";
                                    std::string in2;
                                    std::cin >> in2;
                                    rub = std::atoi(in2.c_str());
                                    if(rub <= 0){
                                        std::cout << "Amount must be greater than zero\n";
                                        break;
                                    }
                                    client->sendUserTransactionRequest(1,rub,usd);
                                    std::cout << "Your buy request has been posted" << '\n';
                                    break;
                                }
                            case 2:
                                {
                                    int64_t usd,rub;
                                    std::cout << "How much USD you want to sell: ";
                                    std::string in1;
                                    std::cin >> in1;
                                    usd = std::atoi(in1.c_str());
                                    if(usd <= 0){
                                        std::cout << "Amount must be greater than zero\n";
                                        break;
                                    }
                                    std::cout << "How much RUB per unit you want to earn: ";
                                    std::string in2;
                                    std::cin >> in2;
                                    rub = std::atoi(in2.c_str());
                                    if(rub <= 0){
                                        std::cout << "Amount must be greater than zero\n";
                                        break;
                                    }
                                    client->sendUserTransactionRequest(0,rub,usd);
                                    std::cout << "Your sell request has been posted" << '\n';
                                    break;
                                }
                            case 3:
                                stage = 0;
                                break;
                            default:
                                std::cout << "Unknown option" << '\n';
                                break;
                        }
                        break;
                    }
                default:
                    break;
            }
        }
	}catch(const std::exception& e){
		std::cerr << e.what() << '\n';
	}
    delete client;
    return 0;
}