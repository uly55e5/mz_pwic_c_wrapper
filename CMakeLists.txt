CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(pwizmzlib)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/dist)
SET(EXT_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
SET(PWIZ_DIR ${EXT_LIB_DIR}/pwiz)


include(ExternalProject)
make_directory(${PWIZ_DIR})
ExternalProject_Add(pwiz_src
        SOURCE_DIR ${PWIZ_DIR}
        GIT_REPOSITORY https://github.com/ProteoWizard/pwiz.git
        GIT_TAG master
        GIT_SHALLOW 1
        GIT_PROGRESS 1
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND tar -xjf libraries/boost_1_76_0.tar.bz2 -C libraries/ &&
                        tar -xjf libraries/zlib-1.2.3.tar.bz2 -C libraries/
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
)

INCLUDE_DIRECTORIES("${PWIZ_DIR}"
                    "${PWIZ_DIR}/libraries/boost_1_76_0/"
                    "${PWIZ_DIR}/libraries/boost_aux"
                    "${PWIZ_DIR}/libraries/zlib-1.2.3")

ADD_COMPILE_DEFINITIONS(WITHOUT_MZ5)

FILE(GLOB PWIZ_MSDATA_SRCS
     "${PWIZ_DIR}/pwiz/data/msdata/*.cpp"
     )
LIST(FILTER PWIZ_MSDATA_SRCS EXCLUDE REGEX "RAMP.*|.*mz5.*|.*Test.cpp")
FILE(GLOB PWIZ_UTIL_SRCS
     "${PWIZ_DIR}/pwiz/utility/misc/SHA1*.cpp"
     "${PWIZ_DIR}/pwiz/utility/misc/BinaryData.cpp"
     "${PWIZ_DIR}/pwiz/utility/misc/IntegerSet.cpp"
     "${PWIZ_DIR}/pwiz/utility/misc/random_access_compressed_ifstream.cpp"
     "${PWIZ_DIR}/pwiz/utility/misc/Base64.cpp"
     "${PWIZ_DIR}/pwiz/utility/chemistry/Chemistry*.cpp"
     "${PWIZ_DIR}/pwiz/utility/misc/Filesystem.cpp"
     "${PWIZ_DIR}/pwiz/utility/misc/IterationListener.cpp"
     "${PWIZ_DIR}/pwiz/utility/misc/String.cpp"
     )
FILE(GLOB PWIZ_XML_SRCS
     "${PWIZ_DIR}/pwiz/utility/minimxml/*.cpp"
     )
FILE(GLOB PWIZ_DATA_SRCS
     "${PWIZ_DIR}/pwiz/data/common/*.cpp"
     )
FILE(GLOB ZLIB_SRCS
     "${PWIZ_DIR}/libraries/zlib-1.2.3/*.c"
     )
LIST(FILTER ZLIB_SRCS EXCLUDE REGEX ".*example.c|.*minigzip.c")
FILE(GLOB BOOST_SRCS
     "${PWIZ_DIR}/libraries/boost_1_76_0/libs/iostreams/src/*.cpp"
     "${PWIZ_DIR}/libraries/boost_1_76_0/libs/filesystem/src/*.cpp"
     "${PWIZ_DIR}/libraries/boost_1_76_0/libs/thread/src/pthread/thread.cpp"
     "${PWIZ_DIR}/libraries/boost_1_76_0/libs/thread/src/pthread/once.cpp"
     "${PWIZ_DIR}/libraries/boost_1_76_0/libs/chrono/src/chrono.cpp"
     )

SET(EXT_LIB_SRCS ${PWIZ_MSDATA_SRCS}
    ${PWIZ_UTIL_SRCS}
    ${PWIZ_XML_SRCS}
    ${PWIZ_DATA_SRCS}
    ${ZLIB_SRCS}
    ${BOOST_SRCS})
LIST(FILTER EXT_LIB_SRCS EXCLUDE REGEX ".*[tT]est.cpp")

SET(SRC_FILES
    pwiz_wrapper.h
    mz_pwiz_c_interface.h
    library.cpp
    pwiz_wrapper.cpp)

ADD_LIBRARY(pwizmzlib ${SRC_FILES} ${EXT_LIB_SRCS})
add_dependencies(pwizmzlib pwiz_src)

INSTALL(TARGETS pwizmzlib DESTINATION debug CONFIGURATIONS Debug)
INSTALL(TARGETS pwizmzlib DESTINATION lib CONFIGURATIONS Release)
