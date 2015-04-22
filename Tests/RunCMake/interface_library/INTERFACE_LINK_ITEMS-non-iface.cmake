add_custom_target(MyTarget)
set_property(TARGET MyTarget PROPERTY INTERFACE_LINK_ITEMS item1)
set_property(TARGET MyTarget APPEND PROPERTY INTERFACE_LINK_ITEMS item2)
