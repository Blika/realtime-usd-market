# Realtime Usd Market (DEMO)
Demo version of a real-time virtual USD market where users can trade their virtual USDs for virtual RUBs.</br>
This project is written in C++ and uses Boost::asio to handle networking.

# Usage
First of all, you need to run `server` executable to start server. Afterwards you are free to launch as many `client` executables as you like.</br>
You can set the desired TCP port in `prefs/settings.json`.</br>

# Server
Server handles incoming connections, users authorization, their trade requests. You must be authorized on server to use it.</br>
Transactions don't account for users' actual balance, thus they can trade as much as they like</br>

All trade requests are perfomed as soon as server gets them. If there isn't a suitable offer, it stay and awaits new requests.</br>
All transactions are perfomed in favour of new requests. If you want to buy some USDs for some price, chances are that there is someone who is selling USDs for even lower price than you're willing to spend.</br>
This way the transaction performs on "seller"'s rules - meaning, you buy their USDs for their price which is lower than yours.</br>
If you want to buy some instead, it's all the same. Nevertheless, if server can't fulfill your request immediately, it will be put on queue. You're still guaranteed to get the price you've asked for in case of successful transactions.</br>

Server also notifies users about their transactions.</br>

There are commands which you can use from server's console.</br>
`announce <message>` sends a message to all connected users. Offline users won't get this message.</br>
`balance <login>` checks USD and RUB balance of a user.</br>
`stop` stops the server.</br>

# Client
Client allows you to send requests to server. You must be authorized to use it. You can launch as many clients as you like but each of them must be logged into different accounts.</br>
Once you've been authorized, you can run a few tests, check your balance, check minimum and maximum of current prices, send trade requests to server.</br>

# Build
Run a build script or build the project yourself. You're going to need `CMake` and `Boost` installed on your OS.
