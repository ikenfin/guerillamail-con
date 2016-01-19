set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -fPIC)
include_directories(${guerillamail_SOURCE_DIR}/lib)
set(GUERILLAMAIL_API_SRC guerillamail-api.c guerillamail-structs.c)
set(GUERILLAMAIL_API_H guerillamail-api.c guerillamail-structs.c)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/bin)

add_library(guerillamail-api SHARED ${GUERILLAMAIL_API_SRC} ${GUERILLAMAIL_API_H})
target_link_libraries(guerillamail-api curl json)
install(TARGETS guerillamail-api DESTINATION ${INSTALL_LIB_DIR})