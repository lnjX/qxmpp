find_package(Doxygen REQUIRED)

if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation.")
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)

add_custom_target(
    doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generate API documentation with Doxygen"
    VERBATIM
)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${CMAKE_INSTALL_DOCDIR})

# QCH file
if(BUILD_QCH)
    if(NOT ECM_FOUND)
        message(FATAL_ERROR "Couldn't find ECM (extra-cmake-modules) for building QCH files")
    endif()

    include(ECMAddQch)
    include(KDEInstallDirs)

    ecm_add_qch(
        QXmpp_QCH
        NAME QXmpp
        BASE_NAME QXmpp
        VERSION ${PROJECT_VERSION}
        ORG_DOMAIN org.qxmpp
        SOURCE_DIRS
            # TODO: Also use doc/xep.doc and others
            ${CMAKE_SOURCE_DIR}/src/base
            ${CMAKE_SOURCE_DIR}/src/client
            ${CMAKE_SOURCE_DIR}/src/server
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Core_QCH
            Qt5Network_QCH
            Qt5Xml_QCH
        INCLUDE_DIRS
            ${CMAKE_BINARY_DIR}/src
        BLANK_MACROS
            QXMPP_EXPORT
            QXMPP_DEPRECATED_SINCE
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        CONFIG_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        COMPONENT Devel
        VERBOSE
    )

    # install targets
    ecm_install_qch_export(
        TARGETS QXmpp_QCH
        FILE QXmppQchTargets.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/qxmpp"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/QXmppQchTargets.cmake\")")
endif()
