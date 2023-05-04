project(lz42lz4)

set(INCLUDE_LIST
    ${ANDROID_TOOL_BASE}/common/local_util
)

set(LINK_DIRS )

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    list(APPEND INCLUDE_LIST /opt/homebrew/include)
    list(APPEND LINK_DIRS /opt/homebrew/lib)
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_LIST})
target_link_directories(${PROJECT_NAME} PRIVATE ${LINK_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE bgrep_e localUtil lz4)
    