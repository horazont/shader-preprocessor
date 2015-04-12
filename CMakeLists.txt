cmake_minimum_required(VERSION 3.0)

set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/")
list(APPEND CMAKE_CXX_FLAGS "-std=c++1y -Wall -Wextra -g -O2")
list(REMOVE_DUPLICATES CMAKE_CXX_FLAGS)

set(SPP_HEADER
  spp/ast.hpp
  spp/context.hpp
  spp/lexer.hpp
  spp/spp.hpp
)
set(SPP_SRC
  src/ast.cpp
  src/context.cpp
)
set(SPP_DUMMY
  src/lexer.ll
  src/parser.yy
)

set(SPP_PARSER_HPP_GEN ${CMAKE_CURRENT_BINARY_DIR}/parser.hpp)
set(SPP_PARSER_CPP_GEN ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
set(SPP_LEXER_CPP_GEN ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)

set(SPP_GEN
  ${SPP_PARSER_HPP_GEN}
  ${SPP_PARSER_CPP_GEN}
  ${SPP_LEXER_CPP_GEN}
)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

add_library(spp STATIC EXCLUDE_FROM_ALL ${SPP_SRC} ${SPP_GEN})

target_include_directories(spp PUBLIC ${INCLUDE_DIRS})
target_include_directories(spp PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(
    OUTPUT ${SPP_LEXER_CPP_GEN}
    COMMAND ${FLEX_EXECUTABLE} --outfile=${SPP_LEXER_CPP_GEN} ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.ll
    DEPENDS ${SPP_PARSER_HPP_GEN} ${SPP_PARSER_CPP_GEN} src/lexer.ll
    COMMENT "Generating lexer"
)
add_custom_command(
    OUTPUT ${SPP_PARSER_HPP_GEN} ${SPP_PARSER_CPP_GEN}
    COMMAND ${BISON_EXECUTABLE} --output=${SPP_PARSER_CPP_GEN} ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.yy
    DEPENDS src/parser.yy
    COMMENT "Generating parser"
)

add_custom_target(dummy_spp_files SOURCES ${SPP_DUMMY} ${SPP_HEADER})


set(SPPTEST_SRC
  spptest/spptest.cpp
)

add_executable(spp_test EXCLUDE_FROM_ALL ${SPPTEST_SRC})
target_link_libraries(spp_test spp)