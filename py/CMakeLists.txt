set(sample_library "qxmpp")
set(bindings_library "QXmpp")
set(wrapped_header ${CMAKE_CURRENT_SOURCE_DIR}/bindings.h)
set(typesystem_file ${CMAKE_CURRENT_SOURCE_DIR}/bindings.xml)
set(generated_sources
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmixiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppstanza_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpp_module_wrapper.cpp
)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

set(PYTHON_CONFIG_SUFFIX ".cpython-37m-x86_64-linux-gnu")
find_package(Shiboken2 REQUIRED)
find_package(PySide2 REQUIRED)

if(NOT EXISTS ${SHIBOKEN_BINARY})
    message(FATAL_ERROR "Shiboken executable not found!")
endif()

set(shiboken_options
    --generator-set=shiboken
    --enable-parent-ctor-heuristic
    --enable-return-value-heuristic
    --use-isnull-as-nb_nonzero
    --avoid-protected-hack
    --enable-pyside-extensions
    -I${CMAKE_SOURCE_DIR}
    -I${CMAKE_SOURCE_DIR}/src/base
    -I${CMAKE_SOURCE_DIR}/src/client
    -I${CMAKE_SOURCE_DIR}/src/server
    -I${CMAKE_BINARY_DIR}/src/base
    -T${CMAKE_SOURCE_DIR}
    -T${PYSIDE_TYPESYSTEMS}
    --output-directory=${CMAKE_CURRENT_BINARY_DIR}
)

foreach(include_dir ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS})
    list(APPEND shiboken_options "-I${include_dir}")
endforeach()

set(generated_sources_dependencies ${wrapped_header} ${typesystem_file})

add_custom_command(OUTPUT ${generated_sources}
	COMMAND ${SHIBOKEN_BINARY} ${shiboken_options} ${wrapped_header} ${typesystem_file}
    DEPENDS ${generated_sources_dependencies}
    IMPLICIT_DEPENDS CXX ${wrapped_header}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running generator for ${typesystem_file}."
)

set(${bindings_library}_sources ${generated_sources})

add_library(${bindings_library} MODULE ${${bindings_library}_sources})

message(${PYSIDE_INCLUDE_DIR})
target_include_directories(${bindings_library} PRIVATE
    ${PYSIDE_INCLUDE_DIR}
    ${PYSIDE_INCLUDE_DIR}/QtCore
    ${PYSIDE_INCLUDE_DIR}/QtNetwork
    ${PYSIDE_INCLUDE_DIR}/QtXml
)
target_include_directories(${bindings_library} PRIVATE ${SHIBOKEN_INCLUDE_DIR})
target_include_directories(${bindings_library} PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(${bindings_library} PRIVATE ${Python3_INCLUDE_DIRS})

target_link_libraries(${bindings_library} PRIVATE ${sample_library})
target_link_libraries(${bindings_library} PRIVATE ${SHIBOKEN_LIBRARY})
target_link_libraries(${bindings_library} PRIVATE ${PYSIDE_LIBRARY})
target_link_libraries(${bindings_library} PRIVATE ${Python3_LIBRARY})

set_property(TARGET ${bindings_library} PROPERTY PREFIX "")
set_property(
    TARGET ${bindings_library}
    PROPERTY
    OUTPUT_NAME "${bindings_library}${PYTHON_EXTENSION_SUFFIX}"
)

add_compile_options(--no-as-needed)

install(TARGETS ${bindings_library} ${sample_library}
        LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
        RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
)
