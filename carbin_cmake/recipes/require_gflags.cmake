
find_path(GFLAGS_INCLUDE_PATH NAMES gflags/gflags.h)
find_library(GFLAGS_LIB NAMES libgflags.a gflags)
include_directories(${GFLAGS_INCLUDE_PATH})
if((NOT GFLAGS_INCLUDE_PATH) OR (NOT GFLAGS_LIB))
    message(FATAL_ERROR "Fail to find gflags")
endif()
include_directories(${GFLAGS_INCLUDE_PATH})
