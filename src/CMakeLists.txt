set(SOURCE_FILES
    cip/connectionManager/ForwardCloseRequest.cpp
    cip/connectionManager/ForwardOpenRequest.cpp
    cip/connectionManager/LargeForwardOpenRequest.cpp
    cip/connectionManager/ForwardOpenResponse.cpp
    cip/connectionManager/NetworkConnectionParametersBuilder.cpp

    cip/CipRevision.cpp
    cip/EPath.cpp
    cip/MessageRouterRequest.cpp
    cip/MessageRouterResponse.cpp
    cip/CipString.cpp

    eip/CommonPacket.cpp
    eip/CommonPacketItem.cpp
    eip/CommonPacketItemFactory.cpp
    eip/EncapsPacket.cpp
    eip/EncapsPacketFactory.cpp

    fileObject/FileObjectEmptyState.cpp
    fileObject/FileObjectLoadedState.cpp
    fileObject/FileObjectNonExistentState.cpp
    fileObject/FileObjectState.cpp
    fileObject/FileObjectUploadInProgressState.cpp

    sockets/BaseSocket.cpp
    sockets/EndPoint.cpp
    sockets/TCPSocket.cpp
    sockets/UDPBoundSocket.cpp
    sockets/UDPSocket.cpp
    sockets/Platform.cpp

    utils/Logger.cpp
    utils/Buffer.cpp

    BaseObject.cpp
    ConnectionManager.cpp
    DiscoveryManager.cpp
    FileObject.cpp
    IdentityObject.cpp
    IOConnection.cpp
    MessageRouter.cpp
    ParameterObject.cpp
    SessionInfo.cpp
)

# if vendor scripts are enabled
if(ENABLE_VENDOR_SRC)
    add_subdirectory(vendor)
endif()

include(GNUInstallDirs)

add_library(${PROJECT_NAME} ${SOURCE_FILES} ${VENDOR_FILES})
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    VERSION ${EIPSCANNER_FULL_VERSION}
    SOVERSION ${EIPSCANNER_MAJOR_VERSION}
)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cip>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cip>
)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cip/connectionManager>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cip/connectionManager>
)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
