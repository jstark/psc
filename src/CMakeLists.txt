set (SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost REQUIRED COMPONENTS program_options)

# frontend
set (FE_DIR "${SRC_DIR}/fe")
set (FE_HDR
    ${FE_DIR}/source.h
    ${FE_DIR}/token.h
    ${FE_DIR}/scanner.h
    ${FE_DIR}/parser.h
    ${FE_DIR}/listeners.h
)
set(FE_SRC
    ${FE_DIR}/source.cpp
    ${FE_DIR}/token.cpp
    ${FE_DIR}/scanner.cpp
    ${FE_DIR}/listeners.cpp
)

# messaging
set (MSG_DIR "${SRC_DIR}/msg")
set (MSG_HDR
    ${MSG_DIR}/message.h
)

# pascal
set (PA_DIR "${SRC_DIR}/pascal")
set (PA_HDR
    ${PA_DIR}/parser.h
    ${PA_DIR}/scanner.h
)
set (PA_SRC
    ${PA_DIR}/parser.cpp
    ${PA_DIR}/scanner.cpp
)

set(HEADERS
    ${FE_HDR}
    ${MSG_HDR}
    ${PA_HDR}
)

set(SOURCES
    psc.cpp
    ${FE_SRC}
    ${PA_SRC}
)

include_directories(.)
add_executable(psc ${HEADERS} ${SOURCES})
target_link_libraries(psc ${Boost_LIBRARIES})
