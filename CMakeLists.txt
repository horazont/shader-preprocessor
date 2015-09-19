cmake_minimum_required(VERSION 3.0)

set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/")

set(SPP_HEADER
  spp/ast.hpp
  spp/context.hpp
  spp/lexer.hpp
  spp/spp.hpp
  spp/loader.hpp
)
set(SPP_SRC
  src/ast.cpp
  src/context.cpp
  src/loader.cpp
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

add_library(spp STATIC ${SPP_SRC} ${SPP_GEN})
set_property(TARGET spp PROPERTY CXX_STANDARD 14)
set_property(TARGET spp PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_options(spp PRIVATE -Wall -Wextra)
target_compile_options(spp PRIVATE $<$<CONFIG:DEBUG>:-ggdb -O2>)
target_compile_options(spp PRIVATE $<$<CONFIG:RELEASE>:-O3>)

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
  tests/main.cpp
  tests/fulltests.cpp
  tests/testdata.hpp
  tests/parsing.cpp
  tests/eval.cpp
)

add_executable(spptests ${SPPTEST_SRC})
set_property(TARGET spptests PROPERTY CXX_STANDARD 14)
set_property(TARGET spptests PROPERTY CXX_STANDARD_REQUIRED ON)
target_compile_options(spptests PRIVATE -Wall -Werror)
target_compile_options(spptests PRIVATE $<$<CONFIG:DEBUG>:-ggdb -O2>)
target_compile_options(spptests PRIVATE $<$<CONFIG:RELEASE>:-O3>)
target_link_libraries(spptests spp)
target_include_directories(spptests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Catch/include)
