cmake_minimum_required(VERSION 3.0.0)
project(parseandroidkernel VERSION 0.1.0)

include(CTest)
enable_testing()

add_definitions(-DDYN_DISASM)
include_directories(hdeA64 localUtil instByEnc opcOperand)
add_subdirectory(hdeA64)
add_subdirectory(instByEnc)
add_subdirectory(opcOperand)
add_subdirectory(parseAndKern)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
