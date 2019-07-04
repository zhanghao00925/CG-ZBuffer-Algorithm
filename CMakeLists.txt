cmake_minimum_required(VERSION 3.7)
project(Computer-Graphics)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF ()

IF (WIN32)
    # Change to your include location
    set(INC_DIR E:\\SDK\\mingw\\include)
    include_directories(${INC_DIR})
    # Change to your lib location
    set(LINK_DIR E:\\SDK\\mingw\\lib)
    link_directories(${LINK_DIR})
    # Change to your OpenCV location
    set(OpenCV_DIR E:\\SDK\\mingw\\OpenCV)
endif ()

find_package(OpenCV REQUIRED)

set(SOURCE_FILES source/main.cpp
        source/PolygonUtil.h source/PolygonUtil.cpp
        source/RasterisationUtil.h source/RasterisationUtil.cpp
        source/BufferUtil.h source/BufferUtil.cpp
        source/SceneUtil.h source/SceneUtil.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})