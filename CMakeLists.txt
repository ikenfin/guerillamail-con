set(PROJECT guerillamail)
set(PROJECT_PATH ${PROJECT})
project(${PROJECT})
cmake_minimum_required(VERSION 2.6)

set(CMAKE_BINARY_DIR bin)
set(INSTALL_BIN_DIR bin)
set(INSTALL_LIB_DIR "lib")

set(DEBUG_DIR debug)

set(IS_DEBUG_BUILD 0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("DEBUG BUILDING")
	message("BUILD PATH - ${CMAKE_CURRENT_LIST_DIR}/${DEBUG_DIR}")
	set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/${DEBUG_DIR})
	set(INSTALL_BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/${DEBUG_DIR})
	set(INSTALL_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/${DEBUG_DIR})
	set(CMAKE_SKIP_BUILD_RPATH TRUE)
	set(BUILD_WITH_INSTALL_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH ${CMAKE_CURRENT_LIST_DIR}/${DEBUG_DIR})
	set(IS_DEBUG_BUILD 1)
else()
	message("RELEASE BUILDING")
	message("make install will need root privileges!")
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/debug-helper/dbg/debug.h.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/debug-helper/dbg/debug.h"
)

include_directories(${guerillamail_SOURCE_DIR}/debug-helper)
link_directories(${guerillamail_SOURCE_DIR}/debug-helper)
add_subdirectory(lib)
add_subdirectory(src)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
