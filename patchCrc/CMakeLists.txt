add_executable(patchCrc main.cpp patchCrc.cpp iterate_dir.cpp)
# # for the android kernel static image parser
target_link_libraries(patchCrc PRIVATE parseAndKern kern_img kernel_block)
# for the instruction parsing
target_link_libraries(patchCrc PRIVATE c_reginst hdeA64 localUtil)
# # for the kernel image parsing
# target_link_libraries(patchCrc PRIVATE kernel_block kern_img)
