cmake_minimum_required (VERSION 2.8)
project (tutor_scheduler)

## ScaLP ##
find_library(SCALP_LIB
        NAMES libScaLP.so libScaLP.dylib
        #HINTS "${SCALP_PREFIX_PATH}/lib"
        DOC "Directory of the ScaLP library")
FIND_PATH(SCALP_H
        ScaLP/Solver.h
        #HINTS "${SCALP_PREFIX_PATH}/include"
        DOC "Path to main ScaLP header file Solver.h"
        )
IF (SCALP_LIB AND SCALP_H)
    message(STATUS "Found ScaLP: ${SCALP_LIB} and ${SCALP_H}")
ELSE (SCALP_LIB AND SCALP_H)
    message(FATAL_ERROR "Could not find ScaLP (${SCALP_LIB} and ${SCALP_H}). Please specify path by setting -DCMAKE_PREFIX_PATH")
ENDIF (SCALP_LIB AND SCALP_H)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB SOURCES
        src/main.cpp
        src/Utility.cpp
        src/Tutor.cpp
        src/TutorScheduler.cpp
        )

add_executable(tutor_scheduler ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${SCALP_H})

target_link_libraries(tutor_scheduler ${SCALP_LIB})

install(FILES ${HEADERS}
        DESTINATION src
        )

install(TARGETS tutor_scheduler
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )