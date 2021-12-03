add_executable(patchCrc main.cpp patchCrc.cpp iterate_dir.cpp)
target_link_libraries(patchCrc PRIVATE parseAndKern)
target_link_libraries(patchCrc PRIVATE hdeA64 c_reginst localUtil)