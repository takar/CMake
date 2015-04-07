# Use a predictable library file name on all platforms.
set(CMAKE_STATIC_LIBRARY_PREFIX lib)
set(CMAKE_STATIC_LIBRARY_SUFFIX .a)

# Convince CMake that it can instruct the linker to search for the
# library of the proper linkage type, but do not really pass flags.
set(CMAKE_EXE_LINK_STATIC_C_FLAGS " ")
set(CMAKE_EXE_LINK_DYNAMIC_C_FLAGS " ")

# Convince CMake that our library is in an implicit linker search directory.
set(dir ${CMAKE_CURRENT_SOURCE_DIR}/lib)
list(APPEND CMAKE_C_IMPLICIT_LINK_DIRECTORIES ${dir})

# Make a link line asking for the linker to search for the library
# look like a missing object file so we will get predictable content
# in the error message.
set(CMAKE_LINK_LIBRARY_FLAG LINKFLAG_)
set(CMAKE_LINK_LIBRARY_SUFFIX _LINKSUFFIX${CMAKE_C_OUTPUT_EXTENSION})

add_executable(main1 main.c)
target_link_libraries(main1 ${dir}/libCMP0060.a)
add_executable(main2 main.c)
target_link_libraries(main2 ${dir}/libCMP0060.a)
