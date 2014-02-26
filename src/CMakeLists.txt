set (SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# boost setup
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})

# frontend
set (FE_DIR "${SRC_DIR}/fe")
set (FE_HDR
    ${FE_DIR}/source.h
    ${FE_DIR}/token.h
    ${FE_DIR}/scanner.h
    ${FE_DIR}/parser.h
)
set(FE_SRC
    ${FE_DIR}/source.cpp
    ${FE_DIR}/token.cpp
    ${FE_DIR}/scanner.cpp
)

# messaging
set (MSG_DIR "${SRC_DIR}/msg")
set (MSG_HDR
    ${MSG_DIR}/message.h
)

# utilities
set (UTILS_DIR "${SRC_DIR}/utils")
set (UTILS_HDR
    ${UTILS_DIR}/var.h
)

# pascal
set (PA_DIR "${SRC_DIR}/pascal")
set (PA_HDR
    ${PA_DIR}/parser.h
    ${PA_DIR}/scanner.h
    ${PA_DIR}/error.h
    ${PA_DIR}/error_handler.h
    ${PA_DIR}/token_type.h
)
set (PA_SRC
    ${PA_DIR}/parser.cpp
    ${PA_DIR}/scanner.cpp
    ${PA_DIR}/error.cpp
    ${PA_DIR}/error_handler.cpp
    ${PA_DIR}/token_type.cpp
)

set(HEADERS
    ${FE_HDR}
    ${MSG_HDR}
    ${PA_HDR}
    ${UTILS_HDR}
)

set(SOURCES
    psc.cpp
    ${FE_SRC}
    ${PA_SRC}
)

include_directories(.)
add_executable(psc ${HEADERS} ${SOURCES})
target_link_libraries(psc ${Boost_LIBRARIES})
