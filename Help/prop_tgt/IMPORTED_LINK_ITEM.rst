IMPORTED_LINK_ITEM
------------------

Specify the link line item for an :ref:`imported <Imported Targets>`
:ref:`Interface Library <Interface Libraries>`.

An interface library has no library file so linking to it normally
adds its usage requirements but does not link to an actual library.
The ``IMPORTED_LINK_ITEM`` property specifies a single item to
be placed on the link line in place of the interface library.
The item may be:

* A full path to a library file such as ``/usr/lib/libfoo.so``.
* A plain library name such as ``foo``.
* A link flag.

See the :command:`target_link_libraries` command for details on
how each type of item is treated, but note that the
``IMPORTED_LINK_ITEM`` property does not recognize the keyword
arguments of that command.  Furthermore, the item specified in
this property is *not* treated as a library target name even if
it happens to name one.

The ``IMPORTED_LINK_ITEM`` property is allowed only on
:ref:`imported <Imported Targets>` :ref:`Interface Libraries`
and is rejected on targets of other types (for which
the :prop_tgt:`IMPORTED_LOCATION` target property may be used).
