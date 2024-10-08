cmake_minimum_required(VERSION 3.11.0)
if(UNIX)
    set(CMAKE_C_COMPILER /usr/bin/gcc)
    set(CMAKE_CXX_COMPILER /usr/bin/g++)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -static-libgcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -static-libgcc -static-libstdc++")
    set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_C_FLAGS} -static-libgcc -s")
    set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS} -static-libgcc -static-libstdc++ -s")
endif()
set(CMAKE_CXX_STANDARD 20)
if(DEBUG)
    project(debug__realtime_usd_market)
    set(CLIENT_EXEC debug__client)
    set(SERVER_EXEC debug__SERVER)
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
else()
    project(realtime_usd_market)
    set(CLIENT_EXEC client)
    set(SERVER_EXEC server)
endif()

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.71.0 COMPONENTS system) 

include_directories(${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE THREAD_POOL ${PROJECT_SOURCE_DIR}/src/threadpool/*.cpp)

file(GLOB_RECURSE SERVER_SOURCES ${PROJECT_SOURCE_DIR}/src/server/*.cpp)
add_executable(${SERVER_EXEC} ${SERVER_SOURCES} ${THREAD_POOL})
target_link_libraries(${SERVER_EXEC} ${Boost_LIBRARIES})

file(GLOB_RECURSE CLIENT_SOURCES ${PROJECT_SOURCE_DIR}/src/client/*.cpp)
add_executable(${CLIENT_EXEC} ${CLIENT_SOURCES} ${THREAD_POOL})
target_link_libraries(${CLIENT_EXEC} ${Boost_LIBRARIES})