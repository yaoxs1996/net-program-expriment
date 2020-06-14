cmake_minimum_required(VERSION 3.10)

project(UNP)

include_directories("./include")
aux_source_directory(./src SRC_LIST)

# Thread
add_executable(threadTest ./thread/threadTest.cpp ./thread/Thread.cpp ./thread/Mutex.cpp ./thread/Cond.cpp)
target_link_libraries (threadTest pthread)
find_package(Threads)

# Threadpool
# add_executable(threadpoolTest ./thread/threadpoolTest.cpp ./thread/Cond.cpp ./thread/Mutex.cpp ./thread/Task.cpp ./thread/ThreadPool.cpp)
# target_link_libraries (threadpoolTest pthread)
# find_package(Threads)

# Epoll server
# add_executable(epollserver ./epoll/epollserver.cpp ./epoll/epoll.cpp ./epoll/socket.cpp)

# Epoll client
# add_executable(epollclient ./epoll/epollclient.cpp ./epoll/epoll.cpp ./epoll/socket.cpp)

# Reactor server
# add_executable(reactor_server ./reactor/reactor_server_test.cpp ./reactor/event_demultiplexer.cpp ./reactor/reactor.cpp ./reactor/global.cpp)

# Reactor client
# add_executable(reactor_client ./reactor/reactor_client_test.cpp ./reactor/event_demultiplexer.cpp ./reactor/reactor.cpp ./reactor/global.cpp)

# Timer
# add_executable(timeTest ./timer/timeTest.cpp ./timer/Timerwheel.cpp)
# target_link_libraries (timeTest pthread)