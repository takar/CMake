add_custom_target(MyCustom)
set_property(TARGET MyCustom PROPERTY IMPORTED_LINK_ITEM item1)
set_property(TARGET MyCustom APPEND PROPERTY IMPORTED_LINK_ITEM item2)
set_property(TARGET MyCustom PROPERTY IMPORTED_LINK_ITEM_DEBUG item1)
set_property(TARGET MyCustom APPEND PROPERTY IMPORTED_LINK_ITEM_DEBUG item2)

add_library(MyStatic STATIC IMPORTED)
set_property(TARGET MyStatic PROPERTY IMPORTED_LINK_ITEM item1)

add_library(MyShared SHARED IMPORTED)
set_property(TARGET MyShared PROPERTY IMPORTED_LINK_ITEM item1)

add_library(MyModule MODULE IMPORTED)
set_property(TARGET MyModule PROPERTY IMPORTED_LINK_ITEM item1)

add_executable(MyExe IMPORTED)
set_property(TARGET MyExe PROPERTY IMPORTED_LINK_ITEM item1)
