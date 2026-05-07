execute_process(
    COMMAND bash "${CMAKE_CURRENT_LIST_DIR}/version.sh"
    OUTPUT_VARIABLE NEW_TIMESTAMP
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
file(WRITE "${OUTPUT_FILE}" "#pragma once\n#define VERSION \"${NEW_TIMESTAMP}\"\n")
