IMPORTED_LINK_ITEM
------------------

Specify the link line item for an :ref:`imported <Imported Targets>`
:ref:`Interface Library <Interface Libraries>`.

An interface library has no library file so linking to it normally
adds its usage requirements but does not link to an actual library.
The ``IMPORTED_LINK_ITEM`` property specifies a single item to
be placed on the link line in place of the interface library.

This property is intended for use in naming libraries provided by
a platform SDK for which the full path to a library file may not
be known.  The item may be a plain library name such as ``foo``
but may *not* be a path (e.g. ``/usr/lib/libfoo.so``) or a flag
(e.g. ``-Wl,...``).  The item is never treated as a library target
name even if it happens to name one.

The ``IMPORTED_LINK_ITEM`` property is allowed only on
:ref:`imported <Imported Targets>` :ref:`Interface Libraries`
and is rejected on targets of other types (for which
the :prop_tgt:`IMPORTED_LOCATION` target property may be used).
