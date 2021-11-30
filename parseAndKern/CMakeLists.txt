add_executable(patchCrc main.cpp patchCrc.cpp parseAndKern.cpp)
target_link_libraries(patchCrc PRIVATE hdeA64 c_reginst localUtil)