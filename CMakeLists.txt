cmake_minimum_required(VERSION 3.9)

project(
  multithread-libevent-example
  VERSION 0.0.1
  LANGUAGES C CXX)

# It's 21 century!
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ImportLibrary)

# valgrind need
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin-strlen -Werror=maybe-uninitialized")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin-strlen")
# sanitize
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize-address-use-after-scope -Werror=maybe-uninitialized"
)

# download libevent
set(EVENT__LIBRARY_TYPE "STATIC")
import_library(
  libevent
  https://github.com/libevent/libevent/releases/download/release-2.1.12-stable/libevent-2.1.12-stable.tar.gz
  b5333f021f880fe76490d8a799cd79f4)
add_subdirectory(${IMPORT_SRC} ${IMPORT_BUILD} EXCLUDE_FROM_ALL)

# download google test
import_library(
  googletest
  https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
  52943a59cefce0ae0491d4d2412c120b)
add_subdirectory(${IMPORT_SRC} ${IMPORT_BUILD} EXCLUDE_FROM_ALL)

import_library(
  spdlog https://github.com/gabime/spdlog/archive/refs/tags/v1.8.5.zip
  06cd154999c8019f52e57afe4a8068f0)
add_subdirectory(${IMPORT_SRC} ${IMPORT_BUILD} EXCLUDE_FROM_ALL)

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ExtNet/ EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ExtNet/)

set(EXEC_NAME NetService)
# set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ExtNet/common/") 
# file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.c") 
add_executable(
  ${EXEC_NAME}
  # main.cc
  # buffer.cc
  # buffer.h
  # thread_pool.h
  # thread_pool.cc
  # dispatcher.h
  # dispatcher.cc
  # handler.cc
  # handler.h
  # listener.cc
  # listener.h

  ExtNet/common/handshake.c
  ExtNet/common/sha1.c
  ExtNet/common/utf8.c
  ExtNet/common/base64.c
  ExtNet/memoryBuff/memoryBuff.cpp
  ExtNet/memoryBuff/stickyWrap.cpp
  ExtNet/common/FdBridge.cpp

  ExtNet/tcp/TcpStickyWrap.cpp
  ExtNet/tcp/TcpConnect.cpp
  ExtNet/tcp/TcpConnectMgr.cpp
  ExtNet/Net/TcpService.cpp

  ExtNet/ws/WsStickyWrap.cpp
  ExtNet/ws/WsConnect.cpp
  ExtNet/ws/WsConnectMgr.cpp
  ExtNet/Net/WsService.cpp
  Net.cc
  )

#set(NETSERVICE TEST)


#target_link_libraries(${NETSERVICE} event_core)
target_link_libraries(${EXEC_NAME} event_core event_pthreads)
# target_link_libraries(${NETSERVICE} event_core event_pthreads event_extra
#                       spdlog::spdlog)
#target_compile_options(TEST PUBLIC -Werror=maybe-uninitialized  -Wall -Wextra -pedantic)
target_compile_options(${EXEC_NAME} PUBLIC -Werror=maybe-uninitialized  -Wall -Wextra -pedantic)

add_executable(
  "client"
  client.cc
)
target_link_libraries("client" event_core event_pthreads)
target_compile_options("client"  PUBLIC -Werror=maybe-uninitialized  -Wall -Wextra -pedantic)
# test
#add_subdirectory(tests)
