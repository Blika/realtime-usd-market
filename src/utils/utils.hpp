#pragma once

#include <iostream>
#include <mutex>
#include <sstream>

static std::mutex coutMutex;

template<typename T>
inline void concat_text(std::stringstream& ss, T&& arg){
    ss << std::forward<T>(arg);
}

template<typename T, typename... Ts>
inline void concat_text(std::stringstream& ss, T&& arg, Ts&&... args){
    ss << std::forward<T>(arg);
    concat_text(ss, std::forward<Ts>(args)...);
}

template<typename T, typename... Ts>
inline void send_error(T&& arg, Ts&&... args){
    std::lock_guard<std::mutex> lock(coutMutex);
    std::stringstream stream;
    stream << "ERROR: ";
    concat_text(stream, arg, std::forward<Ts>(args)...);
    std::cout << stream.str().c_str() << std::endl;
}

template<typename T>
inline void send_error(T&& arg){
    std::lock_guard<std::mutex> lock(coutMutex);
    std::stringstream stream;
    stream << "ERROR: ";
    stream << arg;
    std::cout << stream.str().c_str() << std::endl;
}

template<typename T, typename... Ts>
inline void send_log(T&& arg, Ts&&... args){
    std::lock_guard<std::mutex> lock(coutMutex);
    std::stringstream stream;
    stream << "LOG: ";
    concat_text(stream, arg, std::forward<Ts>(args)...);
    std::cout << stream.str().c_str() << std::endl;
}

template<typename T>
inline void send_log(T&& arg){
    std::lock_guard<std::mutex> lock(coutMutex);
    std::stringstream stream;
    stream << "LOG: ";
    stream << arg;
    std::cout << stream.str().c_str() << std::endl;
}

template<typename T, typename... Ts>
inline void send_debug(T&& arg, Ts&&... args){
    std::lock_guard<std::mutex> lock(coutMutex);
    std::stringstream stream;
    stream << "DEBUG: ";
    concat_text(stream, arg, std::forward<Ts>(args)...);
    std::cout << stream.str().c_str() << std::endl;
}

template<typename T>
inline void send_debug(T&& arg){
    std::lock_guard<std::mutex> lock(coutMutex);
    std::stringstream stream;
    stream << "DEBUG: ";
    stream << arg;
    std::cout << stream.str().c_str() << std::endl;
}
