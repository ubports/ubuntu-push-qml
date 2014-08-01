include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(
    Hellobackend_SRCS
    modules/Hello/backend.cpp
    modules/chatclient.cpp
)

add_library(Hellobackend MODULE
    ${Hellobackend_SRCS}
)

set_target_properties(Hellobackend PROPERTIES
         LIBRARY_OUTPUT_DIRECTORY Hello)

qt5_use_modules(Hellobackend Gui Qml Quick DBus)

# Copy qmldir file to build dir for running in QtCreator
add_custom_target(Hellobackend-qmldir ALL
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/modules/Hello/qmldir ${CMAKE_CURRENT_BINARY_DIR}/Hello
    DEPENDS ${QMLFILES}
)

# Install plugin file
install(TARGETS Hellobackend DESTINATION ${QT_IMPORTS_DIR}/Hello/)
install(FILES   modules/Hello/qmldir DESTINATION ${QT_IMPORTS_DIR}/Hello/)

