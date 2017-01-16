message("Trying to build `guerillamail` makefiles:")

include_directories(${guerillamail_SOURCE_DIR}/lib)
link_directories(${guerillamail_SOURCE_DIR}/lib/guerillamail-api)

set(EXEC_SRC 
	guerillamail-client.c
	console/guerillamail-console-client.c
	console/guerillamail-console-client-common.c
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/bin)

add_definitions(-g)

add_executable(${PROJECT_PATH} ${EXEC_SRC})
target_link_libraries(${PROJECT_PATH} guerillamail-api)
target_link_libraries(${PROJECT_PATH} curl json-c pthread)
install(TARGETS ${PROJECT_PATH} DESTINATION ${INSTALL_BIN_DIR})
