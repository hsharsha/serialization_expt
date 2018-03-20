project(serializers_expt)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Doesn't support non 64-bit platforms")
endif()

include(FindThreads)
include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckCXXCompilerFlag)
include(ExternalProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

CHECK_CXX_COMPILER_FLAG("-std=c++11" CXX11)
if (NOT CXX11)
    message(FATAL_ERROR "C++ compiler doesn't support C++11")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -Wextra")

set(nljson_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/nljson)
ExternalProject_Add(
    nljson
    PREFIX ${nljson_PREFIX}
    URL "https://github.com/nlohmann/json/archive/v3.1.2.tar.gz"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    )
include_directories(${nljson_PREFIX}/src/nljson/single_include)

set(boost_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/boost)
ExternalProject_Add(
    boost
    PREFIX ${boost_PREFIX}
    URL "http://netcologne.dl.sourceforge.net/project/boost/boost/1.62.0/boost_1_62_0.tar.gz"
    URL_MD5 "6f4571e7c5a66ccc3323da6c24be8f05"
    CONFIGURE_COMMAND ${boost_PREFIX}/src/boost/bootstrap.sh --with-libraries=serialization,filesystem,system,program_options,iostreams --prefix=${boost_PREFIX}
    BUILD_COMMAND ./bjam link=static cxxflags='-fPIC'
    INSTALL_COMMAND ./bjam link=static install
    BUILD_IN_SOURCE 1
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
set(Boost_INCLUDE_DIRS ${boost_PREFIX})
set(BOOST_LIBRARIES ${boost_PREFIX}/lib/libboost_serialization.a)
include_directories(${Boost_INCLUDE_DIRS}/include)
message(STATUS "Using Boost from ${Boost_INCLUDE_DIRS}/include")

find_program(AUTORECONF NAMES autoreconf)
if (NOT AUTORECONF)
    message(FATAL_ERROR "Autoreconf not found")
else()
    message(STATUS "Found autoreconf: ${AUTORECONF}")
endif()


set(capnproto_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/capnproto)
ExternalProject_Add(
    capnproto
    PREFIX ${capnproto_PREFIX}
    URL "https://github.com/capnproto/capnproto/archive/v0.6.1.tar.gz"
    URL_MD5 "481d0f2fe24f46ce25234ea9704ef0b4"
    CONFIGURE_COMMAND CXX=${CMAKE_CXX_COMPILER} CC=${CMAKE_C_COMPILER} cd ${capnproto_PREFIX}/src/capnproto/c++ && ${AUTORECONF} -i && cd - && ${capnproto_PREFIX}/src/capnproto/c++/configure --prefix=${capnproto_PREFIX} --libdir=${capnproto_PREFIX}/lib --enable-shared=no
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND $(MAKE) install
    BUILD_IN_SOURCE 1
    LOG_UPDATE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)
include_directories(${capnproto_PREFIX}/include)
set(CAPNPROTO_LIBRARIES ${capnproto_PREFIX}/lib/libcapnp.a ${capnproto_PREFIX}/lib/libkj.a)
set(CAPNPROTO_GENERATOR ${capnproto_PREFIX}/bin/capnp)
set(CAPNPROTO_CPP_GENERATOR ${capnproto_PREFIX}/bin/capnpc-c++)

set(avro_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/avro)
ExternalProject_Add(
    avro
    DEPENDS boost
    PREFIX ${avro_PREFIX}
    URL "http://www.eu.apache.org/dist/avro/avro-1.8.2/avro-src-1.8.2.tar.gz"
    URL_MD5 "cb56b43eb45c3db13a35b56f6df633e2"
    CONFIGURE_COMMAND cmake -DBOOST_INCLUDEDIR=${boost_PREFIX}/include -DBOOST_LIBRARYDIR=${boost_PREFIX}/lib -DBoost_NO_SYSTEM_PATHS=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${avro_PREFIX} -DBUILD_SHARED_LIBS=OFF ${avro_PREFIX}/src/avro/lang/c++/
    # Due to some issues with using statically built boost libraries AND zlib in avro tests
    # build only necessary parts and install them in a very hackish way.
    BUILD_COMMAND $(MAKE) avrocpp_s avrogencpp
    INSTALL_COMMAND mkdir -p ${avro_PREFIX}/include/avro ${avro_PREFIX}/lib ${avro_PREFIX}/bin && cp ${avro_PREFIX}/src/avro-build/avrogencpp ${avro_PREFIX}/bin && cp ${avro_PREFIX}/src/avro-build/libavrocpp_s.a ${avro_PREFIX}/lib && cd ${avro_PREFIX}/src/avro/lang/c++/api/ && cp -r . ${avro_PREFIX}/include/avro/
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
include_directories(${avro_PREFIX}/include)
set(AVRO_LIBRARIES ${avro_PREFIX}/lib/libavrocpp_s.a)
set(AVRO_GENERATOR ${avro_PREFIX}/bin/avrogencpp)

set(flatbuffers_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/external/flatbuffers)
ExternalProject_Add(
    flatbuffers
    PREFIX ${flatbuffers_PREFIX}
    URL "https://github.com/google/flatbuffers/archive/v1.7.1.tar.gz"
    URL_MD5 "81934736f31fbd2cfdb513e71b53b358"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${flatbuffers_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
include_directories(${flatbuffers_PREFIX}/include)
set(FLATBUFFERS_LIBRARIES ${flatbuffers_PREFIX}/lib/libflatbuffers.a)
set(FLATBUFFERS_GENERATOR ${flatbuffers_PREFIX}/bin/flatc)

set(LINKLIBS
    ${CAPNPROTO_LIBRARIES}
    ${AVRO_LIBRARIES}
    ${FLATBUFFERS_LIBRARIES}
)

add_custom_command(
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test.capnp
    COMMAND ${CAPNPROTO_GENERATOR}
    ARGS compile --src-prefix=${CMAKE_CURRENT_SOURCE_DIR} -o ${CAPNPROTO_CPP_GENERATOR} ${CMAKE_CURRENT_SOURCE_DIR}/test.capnp
    OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/test.capnp.c++"
    COMMENT "Executing Cap'n Proto compiler"
)
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/test.capnp.c++
    ${CMAKE_CURRENT_BINARY_DIR}/test.capnp.h
    PROPERTIES GENERATED TRUE
)
set(CAPNPROTO_SERIALIZATION_SOURCES  ${CMAKE_CURRENT_BINARY_DIR}/test.capnp.c++)

add_custom_command(
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test.json
    COMMAND ${AVRO_GENERATOR}
    ARGS --input ${CMAKE_CURRENT_SOURCE_DIR}/test.json --output ${CMAKE_CURRENT_BINARY_DIR}/test_avro.hpp --namespace mynamespace
    OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/test_avro.hpp"
    COMMENT "Executing Avro compiler"
)
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/test_avro.hpp
    PROPERTIES GENERATED TRUE
)

add_custom_command(
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/test.fbs
    COMMAND ${FLATBUFFERS_GENERATOR}
    ARGS --cpp -o ${CMAKE_CURRENT_BINARY_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/test.fbs
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_generated.h"
    COMMENT "Executing FlatBuffers compiler"
)
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/test_generated.h
    PROPERTIES GENERATED TRUE
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(
    serializers_expt
    ${CMAKE_CURRENT_SOURCE_DIR}/serializers_expt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/avro_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/capnproto_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/json_test.cpp
    ${CAPNPROTO_SERIALIZATION_SOURCES}
)
add_custom_target(headergen DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/test_avro.hpp ${CMAKE_CURRENT_BINARY_DIR}/test_generated.h)
add_dependencies(serializers_expt capnproto avro flatbuffers nljson headergen)
target_link_libraries(serializers_expt ${LINKLIBS})
set_target_properties(serializers_expt PROPERTIES COMPILE_FLAGS "-std=c++11")
