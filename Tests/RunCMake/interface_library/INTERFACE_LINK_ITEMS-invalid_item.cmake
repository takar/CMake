enable_language(C)

add_library(SomeIFace INTERFACE)
set_property(TARGET SomeIFace APPEND PROPERTY INTERFACE_LINK_ITEMS bad::item)
add_executable(main main.c)
target_link_libraries(main SomeIFace)
