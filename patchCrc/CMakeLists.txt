add_executable(patchCrc main.cpp patchCrc.cpp iterate_dir.cpp)
target_link_libraries(patchCrc PRIVATE parseAndKern)
target_link_libraries(patchCrc PRIVATE hdeA64 c_reginst localUtil)
# for now i've added these kernel dependencies, considering splitting them.
target_link_libraries(patchCrc PRIVATE krw_util targkrw drv_krw)
