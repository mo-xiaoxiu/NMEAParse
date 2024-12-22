cmake_minimum_required(VERSION 3.10)
project(NMEAParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCE_FILES
    src/NMEAParser.cpp
)

add_library(NMEAParser SHARED ${SOURCE_FILES})

set(INSTALL_LIB_DIR "/usr/lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDE_DIR "/usr/include" CACHE PATH "Installation directory for header files")

if(DEFINED ENV{MY_INSTALL_PREFIX})
    set(INSTALL_LIB_DIR "$ENV{MY_INSTALL_PREFIX}/lib")
    set(INSTALL_INCLUDE_DIR "$ENV{MY_INSTALL_PREFIX}/include")
endif()

install(TARGETS NMEAParser LIBRARY DESTINATION ${INSTALL_LIB_DIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${INSTALL_INCLUDE_DIR})
# 添加测试目标
# enable_testing()
# add_subdirectory(test)
