.. cmake-manual-description: CMake User Interaction Reference

cmake-user-interaction(7)
*************************

.. only:: html

   .. contents::

Introduction
============

Where a software package supplies a CMake-based buildsystem with their
software, the consumer of the software is required to run a CMake user
interaction tool in order to build it.

Well-behaved CMake-based buildsystems do not create any output in the source
directory, so typically, the user performs an out-of-source build and
performs the build there.  First, CMake must be instructed to generate a
suitable buildsystem, then the user invokes a build tool to process that
generated buildsystem.  The generated buildsystem is specific to the machine
used to generate it and is not redistributable.  Each consumer of a provided
software package is required to use CMake to generate a buildsystem specific
to their system.

Generated buildsystems should generally be treated as read-only. The CMake
files as a primary artifact should completely specify the buildsystem and
there should be no reason to populate properties in an IDE for example.

The features and user interfaces described in this manual are available for
all CMake-based build systems by virtue of providing CMake files.

The CMake tooling may report errors to the user when processing provided
CMake files, such as reporting that the compiler is not supported, or the
compiler does not support a required compile option, or a dependency can
not be found.  These errors must be resolved by the user by choosing a
different compiler, installing dependencies, or instructing CMake where to
find them, etc.

Quick Start
-----------

The typical, portable way to build and install any CMake-based software on
any supported platform is:

.. code-block:: console

  $ cd some_software-1.4.2
  $ mkdir build
  $ cd build
  $ cmake .. -DCMAKE_INSTALL_PREFIX=/opt/the/prefix
  $ cmake --build .
  $ cmake --build . --target install

Generating a Buildsystem
========================

There are several user interface tools which may be used to generate a
buildsystem from CMake files.  The :manual:`ccmake(1)` and
:manual:`cmake-gui(1)` tools guide the user through setting the various
necessary options.  The :manual:`cmake(1)` tool can be invoked to specify
options on the command line.  This manual describes options which may be
set using any of the user interface tools, though the mode of setting an
option is different for each tool.

For simplicity, this manual only describes how to set options with
the :manual:`cmake(1)` command line tool, and assumes the software package
is free from bugs preventing typical CMake use.

A simple but typical use of :manual:`cmake(1)` with a fresh copy of software
source code is to create a build directory and invoke cmake there:

.. code-block:: console

  $ cd some_software-1.4.2
  $ mkdir build
  $ cd build
  $ cmake ..

It is recommended to build in a separate directory to the source because
that keeps the source directory pristine, allows for building a single source
with multiple toolchains, and allows easy clearing of build artifacts by
simply deleting the build directory.

The CMake tooling may report warnings which are intended for the provider
of the software, not intended for the consumer of the software.  Such
warnings usually take the form of :manual:`policy <cmake-policies(7)>`
warnings.  Users may disable such warnings by passing the ``-Wno-dev`` flag
to :manual:`cmake(1)`.

Note that spaces in the path to the source directory or build directory can
cause problems if the provided software is not written to carefully quote
each path it uses.  Such issues are usually easy to fix in the provided
CMake files.

Choosing a Generator
--------------------

CMake chooses a generator by default based on the platform.  Usually, the
default generator is sufficient to allow the user to proceed to build the
software.

The user may override the default generator with the ``-G`` option:

.. code-block:: console

  $ cmake .. -G Ninja

The output of ``cmake --help`` includes a list of
:manual:`generators <cmake-generators(7)>` available for the user to
choose from.

On Unix-like systems (including Apple), the :generator:`Unix Makefiles`
generator is used by default.  A variant of that generator can also be used
on Windows in various environments, such as the :generator:`NMake Makefiles`
and :generator:`MinGW Makefiles` generator.  These generators generate
a ``Makefile`` variant which can be executed with ``make``, ``gmake``,
``nmake`` or similar tools.  See the individual generator documentation for
more information on targetted environments and tools.

The :generator:`Ninja` generator is available on all major platforms.
``ninja`` is a build tool similar in use-cases to ``make``, but with a
focus on performance and efficiency.

On Windows, :manual:`cmake(1)` can be used to generate solutions for the
Visual Studio IDE from version 6 to the most recent version.
:manual:`cmake(1)` must be invoked in a command line environment suitable
for executing the compiler which is to be generated for by running the
``vcvarsall.bat`` script, or launching the appropriate command line
environment.

Visual Studio versions may be specified by the product name of the IDE, which
includes a four-digit year.  Aliases are provided for other means by which
Visual Studio versions are sometimes referred to, such as two digits which
correspond to the product version of the VisualC++ compiler, or a
combination of the two:

.. code-block:: console

  $ cmake .. -G "Visual Studio 2013"
  $ cmake .. -G "Visual Studio 12"
  $ cmake .. -G "Visual Studio 12 2013"

If building for a 64 bit environment, ``Win64`` must be appended to the
generator name to generate correct files for the IDE:

.. code-block:: console

  $ cmake .. -G "Visual Studio 2013 Win64"
  $ cmake .. -G "Visual Studio 12 Win64"
  $ cmake .. -G "Visual Studio 12 2013 Win64"

On Apple, the :generator:`Xcode` generator may be used to generate project
files for the Xcode IDE.  The particular version of Xcode used is determined
by the ``xcode-select`` platform tool and the ``DEVELOPER_DIR`` environment
variable.

Note that for all Xcode and Visual Studio generators, the toolchain related
environment variables must be consistent between generating the buildsystem
and executing it.  Executing the build in a different toolchain environment
than it was generated in results in undefined behavior.

CMake also ships some generators which generate a ``Makefile`` or ``Ninja``
based buildsystem, but also generate an IDE project which executes it.  This
allows use of Eclipse with the :generator:`Eclipse CDT4` generator for
example, which allows semantic navigation through the software source files.

Some IDEs such as KDevelop4, QtCreator and CLion have native support for
CMake-based buildsystems.  Those IDEs provide user interface for selecting
an underlying generator to use, typcially a choice between a ``Makefile``
or a ``Ninja`` based generator.

After the first run, :manual:`cmake(1)` may be executed with a path to the
build directory instead of the source directory.  This is most useful if
the build directory is not a subdirectory of the source directory as above,
but in a different location.  Invoking :manual:`cmake(1)` directly after the
first generation is needed only when setting additional options for the
buildsystem generation:

.. code-block:: console

  $ cmake . -DBUILD_EXAMPLES=TRUE

Note that it is not possible to change the generator with ``-G`` after the
first invocation of CMake.  To change the generator, the build directory
must be deleted and the build must be started from scratch.

The CMake Cache
---------------

When CMake is executed, it needs to find the locations of compilers, tools
and dependencies.  It also needs to be able to consistently re-generate
a buildsystem to use the same compile/link flags and paths to
dependencies.  Such parameters are also required to be configurable by the
user because they are paths and options specific to the users system.

When it is first executed, CMake generates a cache file in the build
directory containing key-value pairs for such artifacts.  The cache file
can be viewed or edited by the user by running the :manual:`cmake-gui(1)`
or :manual:`ccmake(1)` tool.  The tools provide user interface for
re-configuring the provided software and re-generating the buildsystem, as
is needed after editing cached values.  Each cache entry may have an
associated short help text which is displayed in the user interface tools.

The cache entries may also have a type to signify how it should be
presented in the user interface.  For example, a cache entry of
type ``BOOL`` can be edited by a checkbox in a user interface, a
``STRING`` can be edited in a text field, and a ``FILEPATH`` while similar
to a ``STRING`` should also provide a way to locate filesystem paths using
a file dialog.  An entry of type ``STRING`` may provide a restricted list of
allowed values which are then provided in a drop-down menu in
the :manual:`cmake-gui(1)` user interface.

On the command line, the ``-D`` option to the :manual:`cmake(1)` tool is
used to specify cache entries, and the ``-U`` option is used to clear
them.  Cache entries may be populated on first run, or any time after
the first run.

The CMake files shipped with a software package may also define options
using the :command:`option` command.  The command creates a cache entry
which can has a type, help text and a default value.  Such cache entries
are typically specific to the provided software and affect the configuration
of the build, such as whether tests and examples are built, whether to build
with exceptions enabled, which major version of Qt to prefer etc.

There is no convention provided by CMake for how such variables are named
between different provided buildsystems, except that variables with the
prefix ``CMAKE_`` usually refer to options provided by CMake itself and
should not be used in provider-specific options, which should use their
own prefix instead.  The :manual:`cmake-gui(1)` tool can display options
in groups defined by their prefix, so it makes sense for providers to
ensure that they use a self-consistent prefix.

The :manual:`cmake(1)` tool allows specifying a file to use to populate
the initial cache using the ``-C`` option.  This can be useful to simplify
commands and scripts which repeatedly require the same cache entries.

Choosing a Toolchain
--------------------

After determining the generator to use, CMake then attempts to determine the
toolchain to use for the build.

CMake generates buildsystem files for multiple languages, so it tries to
find compilers for each language enabled by the :command:`project`
command in the provided software.  As the ``C`` and ``CXX`` languages are
enabled by default, most CMake-based buildsystems will look for compilers
for those languages.

CMake searches for well-known compilers in the ``PATH`` and in other
typical locations.  Several compilers are searched for by default for
a particular language, such as ``g++``, ``cl.exe``, ``clang++`` for the
``CXX`` language.  When CMake is run,
the :variable:`CMAKE_<LANG>_COMPILER` cache entry is populated with the
compiler in use for each language.

The default compiler may be overriden by specifing that cache value in one
of the gui tools or on the command line.  This is typically specified when
first run, along with the generator if overriding that too.  Either a full
path to a compiler may be specified, or a basename may be specified, in
which case CMake will search for it in the ``PATH``:

.. code-block:: console

  $ cmake .. -G "Ninja" \
    -DCMAKE_C_COMPILER=/custom/built/clang \
    -DCMAKE_CXX_COMPILER=/custom/built/clang++

The initial value of the :variable:`CMAKE_<LANG>_COMPILER` for a particular
language may also be specified by populating the environment variable
corresponding to the language:

.. code-block:: console

  $ CXX=g++-4.8 cmake .. -G "Ninja"

If the :variable:`CMAKE_<LANG>_COMPILER` variable for a particular language
changed after the first invocation, CMake will re-run the compiler tests for
the new toolchain.  Note that changing the environment variable and
re-running CMake has no effect, so setting the
:variable:`CMAKE_<LANG>_COMPILER` should be preferred.

In the case of cross compiling provided software, it is usually required to
specify the locations of compilers, the sysroot, custom compile flags and
settings to affect behavior specific to cross-compiling.  Users can maintain
a :variable:`toolchain file <CMAKE_TOOLCHAIN_FILE>` with specific paths to
particular cross-compiling environments.  Such files are usually not
distributable because they only contain content specific to a machine such as
user paths in the filesystem.  For more see :ref:`Cross Compiling Toolchain`.

For most generators, the compiler for each language is a primary
customization point.  For Visual Studio however, the user does not generally
specify the compilers to use, but may specify a particular toolset.  This
can be used to specify a
:ref:`cross compiling toolchain <Cross Compiling Toolchain>` or to specify
the Windows port of the Clang toolchain for use with Visual Studio:

.. code-block:: console

  $ # Build with the clang-cl toolset
  $ cmake.exe .. -G "Visual Studio 2013" -T LLVM-vs2013
  $ # Build targetting Windows XP
  $ cmake.exe .. -G "Visual Studio 2013" -T v120_xp

The :variable:`CMAKE_GENERATOR_TOOLSET` may also be specified in
a :variable:`toolchain file <CMAKE_TOOLCHAIN_FILE>` instead of as a
CMake execution setting.

If using Xcode build tools, the ``Makefile`` and ``Ninja`` generators
determine the compilers to use when CMake is run.  Changing the
``DEVELOPER_DIR`` environment variable, or using ``xcode-select``
to select a different version of Xcode will not affect the toolchain
used to build with those generators as the paths to the tools will be
cached.  If using the :generator:`Xcode` generator, executing the build
in a different toolchain environment than it was generated in results
in undefined behavior.

.. _`Choosing a Build Configuration`:

Choosing a Build Configuration
------------------------------

Often software packages are intended to be built in at least two
configurations: Debug and Release.  These configurations are defined by
a small number of build flags, such as for generating debugging symbols,
defining the ``NDEBUG`` macro or activating a particular optimization level.
Some software also provides extra configurations defined in their CMake
files such as a configuration for compiling in a mode for generating
coverage statistics.

The available configurations known to the provided buildsystem are listed
in the :variable:`CMAKE_CONFIGURATION_TYPES` variable.

The IDE generators, Visual Studio and Xcode, are multi-configuration
generators.  The user does not specify the build configuration when
invoking CMake to generate the project files.  The user instead generates
project files capable of building all configurations listed in the
:variable:`CMAKE_CONFIGURATION_TYPES` variable, and the then chooses
a particular configuration to build afterward.  The IDEs themselves
provide user interface for choosing the build configuration, and
the :manual:`cmake(1)` tool
:ref:`provides a mode <Specifying the Build Configuration>` to build a
particular configuration on the command line.

All other generators are for one particular configuration, determined by
the :variable:`CMAKE_BUILD_TYPE` variable.  The user can set that variable
in a gui tool or on the command line to choose a particular configuration
to generate for:

.. code-block:: console

  $ cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug

The build flags used for a particular configuration and compile language are
specified by the ``CMAKE_<LANG>_FLAGS_<CONFIG>`` variables such
as :variable:`CMAKE_CXX_FLAGS_DEBUG <CMAKE_<LANG>_FLAGS_DEBUG>`.

Configuration names are always handled in a case-insensitive way by
CMake.  That is, ``release``, ``RELEASE`` and ``Release`` are all acceptable
ways of specifying the configuration.

Customizing the Build
---------------------

CMake files in a software package may provide high-level cache options for
some build customization, such as with or without multi-threading support,
enabling compiler warnings etc.  Where high-level interfaces are not
provided, users may specify additional flags when invoking
:manual:`cmake(1)`.

The :variable:`CMAKE_<LANG>_FLAGS` variables such as ``CMAKE_CXX_FLAGS`` can
be populated with a strings containing flags which will be passed to the
compiler for the appropriate language:

.. code-block:: console

  $ cmake .. -DCMAKE_CXX_FLAGS="-Wall -Werror"

For linker flag customization, variables specific to the type of target
being linked are used:

.. code-block:: console

  $ cmake .. -DCMAKE_SHARED_LINKER_FLAGS="-Wl,--no-undefined"

The initial values of the compiler and linker flags are populated with the
content of the environment variables ``CFLAGS``, ``CXXFLAGS`` and
``LDFLAGS``.  These environment variables are considered only on the first
invocation of :manual:`cmake(1)` and are ignored if on subsequent
invocations, so setting the appropriate variables should be preferred.

Because build configurations are defined by variables with a naming
convention, it is often possible to define the build configuration even if
the CMake files do not explicitly provide it:

.. code-block:: console

  $ cmake .. -DCMAKE_BUILD_TYPE=Coverage \
    -DCMAKE_CXX_FLAGS_COVERAGE="-fprofile-arcs -ftest-coverage" \
    -DCMAKE_C_FLAGS_COVERAGE="-fprofile-arcs -ftest-coverage" \
    -DCMAKE_EXE_LINKER_FLAGS_COVERAGE="-fprofile-arcs -ftest-coverage"

Note that both the generic build property variables, such
as ``CMAKE_CXX_FLAGS``, and the config-specific build property variables,
such as ``CMAKE_CXX_FLAGS_RELEASE``, are used when invoking the compiler.
The config-specifc variants are additions, not replacements.

Finding Dependencies
--------------------

CMake files provided with a software package contain instructions for
finding each build dependency.  Some build dependencies are optional
in that the build may succeed with a different feature set if the
dependency is missing, and some dependencies are required.  CMake
searches well-known locations for each dependency, and the provided
software may supply additional hints or locations to CMake to find each
dependency.

If a required dependency is not found by :manual:`cmake(1)`, the cache is
populated with an entry which contains a ``NOTFOUND`` value.  This value
can be replaced by specifying it on the command line, or in
the :manual:`ccmake(1)` or :manual:`cmake-gui(1)` tool.

The cache entries which need to be populated depend on several factors
relating to the dependency.  CMake-aware upstreams may
provide :ref:`Config File Packages` which supply all required information
about the package.  All packaged artifacts such as libraries and header
files have their location specified in
the :ref:`Config File Package <Config File Packages>`, so there is nothing
more CMake is required to find.

CMake searches certain well-known locations for :ref:`Config File Packages`,
and end users can extend the locations searched by populating the
:variable:`CMAKE_PREFIX_PATH` variable, or the environment variable
of the same name.  For each particular package, CMake provides a cache
entry matching the name ``<PACKAGE_NAME>_DIR``, which can be populated on
the command line or using the :manual:`ccmake(1)` or :manual:`cmake-gui(1)`
tool.  The :variable:`CMAKE_PREFIX_PATH` and ``<PACKAGE_NAME>_DIR`` are
dis-similar in that the former is a prefix within which the ``Config.cmake``
file is searched in several locations, whereas the ``<PACKAGE_NAME>_DIR``
is a specific individual directory where the ``Config.cmake`` is expected
to be found directly.

Where dependencies are not CMake-aware, each of their artifacts may need
to be found by specifying their path.  In some cases, this will involve
specifying locations of multiple configurations, such as debug and
release versions.  Some modules are written to find some artifacts
relative to others, so for example specifying the location of ``qmake``
is enough to define the location of the accompanying Qt4 libraries.  Some
find scripts in use by a CMake buildsystem are
:manual:`supplied by CMake <cmake-modules(7)>`, while others are
maintained by the software being built.  Consult the documentation of the
particular module for specific information.

Invoking the Buildsystem
========================

After generating the buildsystem, the software can be built by invoking
the particular build tool.  In the case of the IDE generators, this can
involve loading the generated project file into the IDE to invoke the
build.

For all generators, it is possible to run the underlying build tool after
invoking CMake.  For example, ``make`` may be executed after generating
with the :generator:`Unix Makefiles` generator to invoke the build, or
``ninja`` after generating with the :generator:`Ninja` generator etc.
The IDE buildsystems usually provide command line tooling for building
a project which can also be invoked.

CMake is aware of the specific build tool needed to invoke a build so in
general, to build a buildsystem or project from the command line after
generating, the following command may be invoked in the build directory:

.. code-block:: console

  $ cmake --build .

The ``--build`` flag enables a particular mode of operation for
the :manual:`cmake(1)` tool.  It invokes the :variable:`CMAKE_MAKE_PROGRAM`
command associated with the :manual:`generator <cmake-generators(7)>`, or
the build tool configured by the user.

The ``--build`` mode also accepts the parameter ``--target`` to specify a
particular target to build, for example a particular library, executable or
custom target, or a particular special target like ``install``.

.. code-block:: console

  $ cmake --build . --target myexe

The ``--build`` mode can also pass particular command line options to the
underlying build tool by listing them after ``--``.  This can be useful to
specify options to the build tool, such as parallel jobs, where CMake does
not provide a high-level user interface.

Selecting a Target
------------------

Each executable and library described in the CMake files is a build
target, and the provided package may describe custom targets, either for
internal use, or for user consumption, for example to create documentation.

CMake provides some built-in targets for all buildsystems providing CMake
files.

``all``
  The default target used by ``Makefile`` and ``Ninja`` generators.  Builds
  all targets in the buildsystem, except those which are excluded by
  their :prop_tgt:`EXCLUDE_FROM_ALL` target property
  or :prop_dir:`EXCLUDE_FROM_ALL` directory property.  The name ``ALL_BUILD``
  is used for this purpose for the Xcode and Visual Studio generators.
``help``
  Lists the targets available for build.  This target is available when
  using the :generator:`Unix Makefiles` or :generator:`Ninja` generator, and
  the exact output is tool-specific.
``clean``
  Delete built object files and other output files.  The ``Makefile`` based
  generators create a ``clean`` target per directory, so that an individual
  directory can be cleaned.  The ``Ninja`` tool provides its own granular
  ``-t clean`` system.
``test``
  Runs tests.  This target is only automatically available if the CMake
  files provide CTest-based tests.
``install``
  Installs the software.  This target is only automatically available if
  the software defines install rules with the :command:`install`
  command.  See also `Software Installation`_.
``package``
  Creates a binary package.  This target is only automatically available
  if the CMake files provide CPack-based packages.
``package_source``
  Creates a source package.  This target is only automatically available
  if the CMake files provide CPack-based packages.

For ``Makefile`` based systems, ``/fast`` variants of binary build targets
are provided. The ``/fast`` variants are used to build the specified target
without regard for its dependencies.  The dependencies are not checked and
are not rebuilt if out of date.  The :generator:`Ninja` generator is
sufficiently fast at dependency checking that such targets are not provided.

``Makefile`` based systems also provide build-targets to preprocess,
assemble and compile individual files in a particular directory.

.. code-block:: console

  $ make foo.cpp.i
  $ make foo.cpp.s
  $ make foo.cpp.o

The file extension is built into the name of the target because another file
with the same name but a different extension may exist.  However,
build-targets without the file extension are also provided.

.. code-block:: console

  $ make foo.i
  $ make foo.s
  $ make foo.o

In buildsystems which contain ``foo.c`` and ``foo.cpp``, building
the ``foo.i`` target will preprocess both files.

.. _`Specifying the Build Configuration`:

Specifying a Build Configuration
--------------------------------

The mode also accepts a ``--config`` parameter in the case of multi-config
IDE projects to specify which particular configuration to build.

.. code-block:: console

  $ cmake --build . --target myexe --config Release

The ``--config`` option has no effect if the generator generates a
buildsystem specific to a configuration which is
:ref:`chosen when invoking cmake <Choosing a Build Configuration>` with
the :variable:`CMAKE_BUILD_TYPE` variable.

Specifying a Build Program
--------------------------

The program invoked by the ``--build`` mode is determined by the
:variable:`CMAKE_MAKE_PROGRAM` variable.  For some generators, the
particular program does not need to be configured.

===================== =========================== ===========================
      Generator           Default make program           Alternatives
===================== =========================== ===========================
 XCode                 ``xcodebuild``
 Unix Makefiles        ``make``
 NMake Makefiles       ``nmake``                   ``jom``
 NMake Makefiles JOM   ``jom``                     ``nmake``
 MinGW Makefiles       ``mingw32-make``
 MSYS Makefiles        ``make``
 Ninja                 ``ninja``
 Visual Studio         ``msbuild``                ``devenv``
 Watcom WMake          ``wmake``
===================== =========================== ===========================

The ``jom`` tool is capable of reading makefiles of the ``NMake`` flavor
and building in parallel, while the ``nmake`` tool always builds
serially.  After generating with the :generator:`NMake Makefiles` generator
a user can run ``jom`` instead of ``nmake``.  The ``--build`` mode would
also use ``jom`` if the :variable:`CMAKE_MAKE_PROGRAM` was set to ``jom``
while using the :generator:`NMake Makefiles` generator, and as a
convenience, the :generator:`NMake Makefiles JOM` generator is provided
to find ``jom`` in the normal way and use it as
the :variable:`CMAKE_MAKE_PROGRAM`. For completeness, ``nmake`` is an
alternative tool which can process the output of
the :generator:`NMake Makefiles JOM` generator, but doing so would be a
pessimisation.

The Visual Studio generators use either ``msbuild`` or ``devenv`` by
default, depending on the version and the enabled toolsets.

Software Installation
=====================

The :variable:`CMAKE_INSTALL_PREFIX` variable can be set in the CMake cache
to specify where to install the provided software.  If the provided
software has install rules, specified using the :command:`install` command,
they will install artifacts into that prefix.  On Windows, the default
installation location corresponds to the ``ProgramFiles`` system directory
which may be architecture specific.  On Unix hosts, ``/usr/local`` is the
default installation location.

The :variable:`CMAKE_INSTALL_PREFIX` variable always refers to the
installation prefix on the target filesystem.  The CMake RPath-related
features ensure that this is the string written into the installed binary,
and the buildsystem might write it into the compiled binary in some cases.

In cross-compiling or packaging scenarios where the sysroot is read-only
or where the sysroot should otherwise remain pristine,
the :variable:`CMAKE_STAGING_PREFIX` variable can be set to a location to
actually install the files.

The commands:

.. code-block:: console

  $ cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_SYSROOT=$HOME/root \
    -DCMAKE_STAGING_PREFIX=/tmp/package
  $ cmake --build .
  $ cmake --build . --target install

result in files being installed to paths such
as ``/tmp/package/lib/libfoo.so`` on the host machine, with RPath
information on the installed binaries prefixed at ``/usr/local``.  The
``/usr/local`` location on the host machine is not affected.

Some provided software may specify ``uninstall`` rules, but CMake does not
generate such rules by default itself.
