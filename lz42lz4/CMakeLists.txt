add_executable(lz42lz4 main.cpp)
target_link_libraries(lz42lz4 PRIVATE bgrep_e localUtil lz4)
target_include_directories(lz42lz4 PRIVATE
    ${ANDROID_TOOL_BASE}/common/local_util)