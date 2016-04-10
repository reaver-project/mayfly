# Mayfly

Mayfly is the Reaver Project's testing framework. It's meant to be lightweight and
benefit from as many modern C++ features as possible. Primary goals for Mayfly are
to be fast, generate readable outputs (and TeamCity-ready results) and later get
closely integrated with Despayre, the Reaver Project's build system.

## Supported compilers

Currently supported compilers include GCC 5.1+ and Clang 3.7+, both with libstdc++
and libc++, and with Boost 1.60.0+. It probably also works on lower versions, but
no such compatibility will be checked and maintained.

CI status can be checked on [Reaver Project's TeamCity server](http://ci.reaver-project.org/viewType.html?buildTypeId=mayfly_Tests&guest=1).
