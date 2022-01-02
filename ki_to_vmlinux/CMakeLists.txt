add_executable(ki_to_vmlinux main.cpp)
# for the kernel image parsing
target_link_libraries(ki_to_vmlinux PRIVATE parseAndKern kernel_block kern_img)
target_link_libraries(ki_to_vmlinux PRIVATE c_reginst hdeA64 localUtil)
# for now i've added these kernel dependencies, considering splitting them.
# target_link_libraries(ki_to_vmlinux PRIVATE krw_util targkrw drv_krw)

