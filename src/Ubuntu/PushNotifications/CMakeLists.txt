project(ubuntu-push-notifications-plugin)

execute_process(
    COMMAND qmake -query QT_INSTALL_QML
    OUTPUT_VARIABLE QT_INSTALL_QML
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(QML_IMPORT PushNotifications)
set(IMPORTS_DIR "${QT_INSTALL_QML}/Ubuntu/${QML_IMPORT}")
set(PLUGIN Ubuntu${QML_IMPORT})
set(PLUGIN_SRC plugin.cpp pushclient.cpp)
list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

add_library(${PLUGIN} MODULE ${PLUGIN_SRC})

qt5_use_modules(${PLUGIN} Core Gui Qml DBus)

file(GLOB QML_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.qml qmldir *.js)
install(TARGETS ${PLUGIN} DESTINATION ${IMPORTS_DIR})
install(FILES ${QML_FILES} DESTINATION ${IMPORTS_DIR})

add_custom_command(
    TARGET ${PLUGIN}
    POST_BUILD
    COMMAND "qmlplugindump" "Ubuntu.${QML_IMPORT}" "0.1" "${CMAKE_BINARY_DIR}/src" ">" "plugins.qmltypes"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plugins.qmltypes
    DESTINATION ${IMPORTS_DIR})

if(NOT ${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  # copy qml files over to build dir to be able to import them uninstalled
  set(copied ${QML_FILES})
  foreach(_file ${copied})
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_file}
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_file}
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${_file} ${CMAKE_CURRENT_BINARY_DIR}/${_file})
  endforeach(_file)
  add_custom_target(copy_files_to_build_dir DEPENDS ${copied})
  add_dependencies(${PLUGIN} copy_files_to_build_dir)
endif()
