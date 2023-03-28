project(count_ksymtabstr)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${ANDROID_TOOL_BASE}/common/local_util
)

target_link_libraries(${PROJECT_NAME} PRIVATE localUtil)
