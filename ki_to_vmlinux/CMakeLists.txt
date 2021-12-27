add_executable(ki_to_vmlinux main.cpp)
target_link_libraries(ki_to_vmlinux PRIVATE parseAndKern hdeA64 c_reginst localUtil)
# for now i've added these kernel dependencies, considering splitting them.
# target_link_libraries(ki_to_vmlinux PRIVATE krw_util targkrw drv_krw)
