# Bundled CMake module fallback

This directory retains only the CMake modules and templates imported by the launcher build:

- `ECMQueryQt.cmake` and its `QtVersionOption.cmake` dependency.
- `ECMQtDeclareLoggingCategory.cmake` and its C++/header templates.

The launcher first uses an installed Extra CMake Modules package when available. These files are
the fallback for environments where ECM is not installed. Other ECM modules, its standalone
build/documentation project, CI configuration and module test suite are not part of this fork's
production build.
