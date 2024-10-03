#include "command_executor.hpp"
#include "../server/server.hpp"
#include "../user/user.hpp"
#include "../../utils/utils.hpp"
#include "../../packets/packets.hpp"
#include <boost/algorithm/string.hpp>

namespace realtime_usd_market{

    CommandExecutor::CommandExecutor(){}
    CommandExecutor::~CommandExecutor(){}

    void CommandExecutor::execute(const std::string& txt){
        std::vector<std::string> cmd;
        boost::split(cmd, txt, boost::is_any_of(" "));
        if(cmd.empty()){
            send_log("unknown command");
            return;
        }
        if(cmd[0] == "announce"){
            if(cmd.size() < 2){
                send_log("announce <message>");
                return;
            }
            cmd.erase(cmd.begin());
            std::string n = boost::algorithm::join(cmd," ");
            AnnounceMessage pk(n);
            Server::getInstance()->broadcastPacket(&pk);
        }else if(cmd[0] == "balance"){
            if(cmd.size() < 2){
                send_log("balance <login>");
                return;
            }
            User* user = Server::getInstance()->getUser(cmd[1]);
            if(user == nullptr){
                send_log("user does not exist");
                return;
            }
            send_log(user->getLogin()," has ",user->getUsd()," USD, ",user->getRub()," RUB");
        }else if(cmd[0] == "stop"){
            Server::getInstance()->shutdown();
        }else{
            send_log("unknown command ",cmd[0]);
        }
    }

}