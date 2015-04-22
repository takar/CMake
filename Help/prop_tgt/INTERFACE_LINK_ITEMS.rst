INTERFACE_LINK_ITEMS
--------------------

Specify link line items for an :ref:`Interface Library <Interface Libraries>`.

An interface library has no library file so linking to it normally
adds its usage requirements but does not link to an actual library.
The ``INTERFACE_LINK_ITEMS`` property specifies a
:ref:`;-list <CMake Language Lists>` of items to be placed on the
link line in place of the interface library.  One may use
:manual:`generator expressions <cmake-generator-expressions(7)>`.
Each item may be:

* A full path to a library file such as ``/usr/lib/libfoo.so``.
* A plain library name such as ``foo``.
* A link flag.

See the :command:`target_link_libraries` command for details on
how each type of item is treated, but note that the
``INTERFACE_LINK_ITEMS`` property does not recognize the keyword
arguments of that command.  Furthermore, items specified in this
property are *not* treated as library target names even if they
happen to name one.  Items may *not* contain ``::`` because CMake
assumes such names refer to :ref:`Alias Targets` or
:ref:`Imported Targets` whether they are defined or not.

The ``INTERFACE_LINK_ITEMS`` property is allowed only on
:ref:`Interface Libraries` and is rejected on targets of other
types.
