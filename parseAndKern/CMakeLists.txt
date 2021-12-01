add_executable(patchCrc main.cpp patchCrc.cpp parseAndKern.cpp iterate_dir.cpp)
add_executable(modpost_dup modpost_dup.c sumversion_dup.c)
target_link_libraries(patchCrc PRIVATE hdeA64 c_reginst localUtil)