find_program(BASH_EXECUTABLE NAMES bash)

set(NEW_TIMESTAMP "")

if (BASH_EXECUTABLE)
    execute_process(
        COMMAND ${BASH_EXECUTABLE} "${CMAKE_CURRENT_LIST_DIR}/version.sh"
        OUTPUT_VARIABLE NEW_TIMESTAMP
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE BASH_RESULT
    )
endif()

if (NOT BASH_EXECUTABLE OR NOT BASH_RESULT EQUAL 0)
    # Fallback to simple timestamp if bash is missing or fails (e.g. on standard Windows)
    string(TIMESTAMP NEW_TIMESTAMP "%y.%m%d.%H%M")
endif()

file(WRITE "${OUTPUT_FILE}" "#pragma once\n#define VERSION \"${NEW_TIMESTAMP}\"\n")
