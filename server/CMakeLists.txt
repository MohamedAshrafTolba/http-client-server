cmake_minimum_required( VERSION 3.12 )

project( http-server )

set(CMAKE_CXX_STANDARD 11)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra" )

set( HEADERS Server.h ../HttpRequest.h ../HttpWorkerThread.h ../ServerSocket.h ../Socket.h ../strutil.h )

set( SOURCES Server.cpp ../HttpRequest.cpp ../HttpWorkerThread.cpp ../ServerSocket.cpp ../Socket.cpp ../strutil.cpp )

find_package(Boost REQUIRED COMPONENTS system filesystem)

add_executable( http-server ${HEADERS} ${SOURCES})

target_link_libraries( http-server Boost::system Boost::filesystem )