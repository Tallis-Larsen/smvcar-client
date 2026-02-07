cmake_minimum_required(VERSION 3.16)
project(smvcar-client)
set(CMAKE_CXX_STANDARD 17)

# Automates the Qt compiler
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "include/*.h") # Header files need to be included so that the QT MOC picks up on them
add_executable(smvcar-client ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(smvcar-client PRIVATE include)
target_link_libraries(smvcar-client PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)