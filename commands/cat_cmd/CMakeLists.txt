add_executable(cat cat_cmd.cpp) 
target_link_libraries(cat PRIVATE build_in_basic file_operation)