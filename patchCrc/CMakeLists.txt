add_executable(patchCrc main.cpp patchCrc.cpp iterate_dir.cpp)
# for the android kernel static image parser
target_link_libraries(patchCrc PRIVATE parseAndKern)
# for the instruction parsing
target_link_libraries(patchCrc PRIVATE hdeA64 c_reginst localUtil)
# for the kernel image parsing
target_link_libraries(patchCrc PRIVATE kernel_block kern_img)
