string(TIMESTAMP NEW_TIMESTAMP "%y.%m%d.%H%M")
file(WRITE "${OUTPUT_FILE}" "#pragma once\n#define VERSION \"${NEW_TIMESTAMP}\"\n")
