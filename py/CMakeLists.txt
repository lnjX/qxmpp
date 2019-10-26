set(sample_library "qxmpp")
set(bindings_library "QXmpp")
set(wrapped_header ${CMAKE_CURRENT_SOURCE_DIR}/bindings.h)
set(typesystem_file ${CMAKE_CURRENT_SOURCE_DIR}/bindings.xml)
set(generated_sources
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpparchivechatiq_wrapper.cpp
#     ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpparchivechat_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpparchivelistiq_wrapper.cpp
#     ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpparchivemanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpparchivemessage_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpparchiveprefiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpparchiveremoveiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpparchiveretrieveiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppbindiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppbookmarkconference_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppbookmarkmanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppbookmarkset_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppbookmarkurl_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppbytestreamiq_streamhost_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppbytestreamiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppcallmanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppcall_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppcarbonmanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppclientextension_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppdataform_field_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppdataform_mediasource_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppdataform_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppdialback_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppdiscoveryiq_identity_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppdiscoveryiq_item_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppdiscoveryiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppdiscoverymanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppelement_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppentitytimeiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppentitytimemanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppextendedaddress_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpphttpuploadrequestiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpphttpuploadslotiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppicecomponent_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppiceconnection_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppincomingclient_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppincomingserver_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppinvokable_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppjingleiq_content_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppjingleiq_reason_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppjingleiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppjinglepayloadtype_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpploggable_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpplogger_wrapper.cpp
#     ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmammanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmessagereceiptmanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmessage_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmixinfoitem_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmixiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmixparticipantitem_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpp_module_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmucadminiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmucitem_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmucmanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmucowneriq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppmucroom_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpppasswordchecker_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpppasswordreply_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpppasswordrequest_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpppingiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpppresence_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpppubsubiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpppubsubitem_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppregisteriq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppremotemethod_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppresultsetquery_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppresultsetreply_wrapper.cpp
#     ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprosteriq_item_wrapper.cpp
#     ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprosteriq_wrapper.cpp
#     ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprostermanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprpcerroriq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprpcinvokeiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprpcmanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprpcmarshaller_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprpcresponseiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprtcpsenderinfo_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprtcpsourcedescription_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprtpaudiochannel_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprtpchannel_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprtppacket_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpprtpvideochannel_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppserverextension_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppserverplugininterface_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppserverplugin_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppsessioniq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppsocksclient_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppsocksserver_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppstanza_error_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppstanza_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppstreamfeatures_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppstream_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpptransferfileinfo_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpptransferjob_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpptransfermanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppuploadservice_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmpputils_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppvcardaddress_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppvcardemail_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppvcardiq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppvcardmanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppvcardorganization_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppvcardphone_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppversioniq_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppversionmanager_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppvideoformat_wrapper.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${bindings_library}/qxmppvideoframe_wrapper.cpp
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
