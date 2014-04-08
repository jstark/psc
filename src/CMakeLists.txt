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

# im
set (IM_DIR "${SRC_DIR}/im")
set (IM_HDR
    ${IM_DIR}/common.h  
	  ${IM_DIR}/symbol_table.h
	  ${IM_DIR}/icode.h
	  ${IM_DIR}/xref.h
    ${IM_DIR}/parse_tree_printer.h
)
set(IM_SRC
	  ${IM_DIR}/symbol_table.cpp
	  ${IM_DIR}/icode.cpp
	  ${IM_DIR}/xref.cpp
    ${IM_DIR}/parse_tree_printer.cpp
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
    ${UTILS_DIR}/fwd.h
)

# pascal
set (PA_DIR "${SRC_DIR}/pascal")
set (PA_HDR
    ${PA_DIR}/parser.h
    ${PA_DIR}/scanner.h
    ${PA_DIR}/error.h
    ${PA_DIR}/error_handler.h
    ${PA_DIR}/token_type.h
	${PA_DIR}/stmnt_parser.h
	${PA_DIR}/nt_parser.h
	${PA_DIR}/compound_parser.h
	${PA_DIR}/assignment_parser.h
	${PA_DIR}/expr_parser.h
	${PA_DIR}/while_parser.h
	${PA_DIR}/repeat_parser.h
	${PA_DIR}/for_parser.h
)
set (PA_SRC
    ${PA_DIR}/parser.cpp
    ${PA_DIR}/scanner.cpp
    ${PA_DIR}/error.cpp
    ${PA_DIR}/error_handler.cpp
    ${PA_DIR}/token_type.cpp
	${PA_DIR}/stmnt_parser.cpp
	${PA_DIR}/nt_parser.cpp
	${PA_DIR}/compound_parser.cpp
	${PA_DIR}/assignment_parser.cpp
	${PA_DIR}/expr_parser.cpp
	${PA_DIR}/while_parser.cpp
	${PA_DIR}/repeat_parser.cpp
	${PA_DIR}/for_parser.cpp
)

#backend
set (BE_DIR "${SRC_DIR}/be")
set (BE_HDR 
	${BE_DIR}/backend.h
	${BE_DIR}/interpreter.h
	${BE_DIR}/rt_error.h
	${BE_DIR}/rt_error_handler.h
)
set (BE_SRC
	${BE_DIR}/backend.cpp
	${BE_DIR}/interpreter.cpp
	${BE_DIR}/rt_error.cpp
	${BE_DIR}/rt_error_handler.cpp
)

set(HEADERS
    ${FE_HDR}
	${IM_HDR}
    ${MSG_HDR}
    ${PA_HDR}
    ${UTILS_HDR}
	${BE_HDR}
)

set(SOURCES
    psc.cpp
    ${FE_SRC}
    ${IM_SRC}
    ${PA_SRC}
	${BE_SRC}
)

include_directories(.)
add_executable(psc ${HEADERS} ${SOURCES})
target_link_libraries(psc ${Boost_LIBRARIES})

source_group("frontend" FILES ${FE_HDR} ${FE_SRC})
source_group("intermediate" FILES ${IM_HDR} ${IM_SRC})
source_group("backend"  FILES ${BE_HDR} ${BE_SRC})
source_group("pascal"  FILES ${PA_HDR} ${PA_SRC})
source_group("various" FILES ${UTILS_HDR} ${MSG_HDR} psc.cpp)
