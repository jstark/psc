project(psc)
cmake_minimum_required(VERSION 2.8)

# enable C++11 features for gcc & clang
if (CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    # also use libc++ in MAC OS X
    if (CMAKE_SYSTEM_NAME MATCHES Darwin)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES Windows)
	add_definitions("/MP")
endif()

add_subdirectory(src)

