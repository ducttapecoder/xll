cmake_minimum_required(VERSION 3.14)

project(QFLibrary)


set(XLL_INTERFACE_FILES ${CMAKE_SOURCE_DIR}/source/cppinterface.h)
set(XLL_DESTINATION_FILE ${CMAKE_BINARY_DIR}/generated/xlwWrapper.cpp)

set(XLL_IMPLEMENTATION_FILES source/source.cpp)


add_custom_command(OUTPUT ${XLL_DESTINATION_FILE}
    COMMAND ${INTERFACEGENERATOR_EXECUTABLE} ARGS ${XLL_INTERFACE_FILES} ${XLL_DESTINATION_FILE}
    DEPENDS ${XLL_INTERFACE_FILES}
    VERBATIM
    )

add_library(${PROJECT_NAME} SHARED ${XLL_DESTINATION_FILE} ${XLL_IMPLEMENTATION_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".xll")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)

target_link_directories(${PROJECT_NAME} PUBLIC ${XLW_LIB_DIR})
target_link_libraries(${PROJECT_NAME} ${XLW_LIB} )
target_include_directories(${PROJECT_NAME} PUBLIC ${XLW_INCLUDE_DIR} PRIVATE ${CMAKE_SOURCE_DIR}/source)

install(TARGETS ${PROJECT_NAME} EXPORT "${PROJECT_NAME}.xll" DESTINATION bin)