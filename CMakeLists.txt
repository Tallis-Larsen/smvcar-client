cmake_minimum_required(VERSION 3.16)
project(smvcar-client)
set(CMAKE_CXX_STANDARD 17)

# Automates Qt tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(smvcar-client src/main.cpp)
target_include_directories(smvcar-client PRIVATE include)
target_link_libraries(smvcar-client PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
