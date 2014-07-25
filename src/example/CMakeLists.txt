project(ubuntu-push-example)

include_directories(${Qt5Quick_PRIVATE_INCLUDE_DIRS})

file(GLOB QML_FILES *.qml)
install(FILES ${QML_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/ubuntu-push-example)

set(DESKTOP_FILE ubuntu-push-example.desktop)
set(EXEC "qmlscene ${CMAKE_INSTALL_DATADIR}/ubuntu-push-example/${DESKTOP_FILE} %f" )
configure_file(${DESKTOP_FILE}.in.in ${DESKTOP_FILE}.in @ONLY)
file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE})
file(STRINGS ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in DESKTOP_FILE_CONTENTS)
foreach(LINE ${DESKTOP_FILE_CONTENTS})
    string(REGEX REPLACE "tr\\\(\"(.*)\"\\\)" "\\1" LINE "${LINE}")
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE} "${LINE}\n")
endforeach(LINE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

