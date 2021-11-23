cmake_minimum_required(VERSION 3.0.0)
project(parseandroidkernel VERSION 0.1.0)

include(CTest)
enable_testing()

# SET(CMAKE_C_FLAGS_DEBUG "-O0")
# SET(CMAKE_CXX_FLAGS_DEBUG "-O0")
add_definitions(-DDYN_DISASM)
include_directories(hde_armv8 localUtil c_reginst)
add_subdirectory(hde_armv8)
add_subdirectory(c_reginst)
add_subdirectory(parseAndKern)
add_subdirectory(localUtil)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
