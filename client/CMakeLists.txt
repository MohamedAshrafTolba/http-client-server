cmake_minimum_required( VERSION 3.12 )

project( http-client )

set(CMAKE_CXX_STANDARD 11)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra" )

set( HEADERS Client.h ../HttpRequest.h ../HttpWorkerThread.h ../Socket.h ../strutil.h )

set( SOURCES main.cpp Client.cpp ../HttpRequest.cpp ../HttpWorkerThread.cpp ../Socket.cpp ../strutil.cpp )

find_package( Boost REQUIRED COMPONENTS system filesystem )

add_executable( http-client ${HEADERS} ${SOURCES} )

target_link_libraries( http-client Boost::system Boost::filesystem )