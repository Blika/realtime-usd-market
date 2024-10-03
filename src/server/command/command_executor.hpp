#pragma once

#include <queue>
#include <string>

namespace realtime_usd_market{

    class CommandExecutor{
        public:
            CommandExecutor();
            ~CommandExecutor();

            static void execute(const std::string& cmd);
    };
    
}