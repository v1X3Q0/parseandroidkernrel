add_executable(ki_to_vmlinux main.cpp)
target_link_libraries(ki_to_vmlinux PRIVATE parseAndKern hdeA64 c_reginst localUtil)
