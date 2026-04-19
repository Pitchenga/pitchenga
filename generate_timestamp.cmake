string(TIMESTAMP NEW_TIMESTAMP "%Y-%m-%d %H:%M:%S")
file(WRITE "${OUTPUT_FILE}" "#pragma once\n#define BUILD_TIMESTAMP \"${NEW_TIMESTAMP}\"\n")
