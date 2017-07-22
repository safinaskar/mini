cmake_minimum_required (VERSION 3.1) # 3.1 adds C_STANDARD
project (mini LANGUAGES C CXX)

add_library (minilib SHARED getdelim.c strtok_r.c util.c mini.c)
target_link_libraries (minilib -lreadline ${CMAKE_DL_LIBS})
set_property (TARGET minilib PROPERTY C_STANDARD 99)

add_executable (mini main.cpp)
target_link_libraries (mini minilib -lsh)
set_property (TARGET mini PROPERTY CXX_STANDARD 14)
set_target_properties (mini PROPERTIES ENABLE_EXPORTS on)

install (FILES mini.h DESTINATION include)
install (TARGETS minilib DESTINATION lib)
install (TARGETS mini DESTINATION bin)
