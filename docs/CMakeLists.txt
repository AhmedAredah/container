set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating documentation with Doxygen"
    VERBATIM)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mainpage.dox
               ${CMAKE_CURRENT_BINARY_DIR}/mainpage.dox
               COPYONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dark_theme.css
               ${CMAKE_CURRENT_BINARY_DIR}/dark_theme.css
               COPYONLY)