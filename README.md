# Mayfly

Mayfly is the Reaver Project's testing framework. It's meant to be lightweight and
benefit from as many modern C++ features as possible. Primary goals for Mayfly are
to be fast, generate readable outputs (and TeamCity-ready results) and later get
closely integrated with Despayre, the Reaver Project's build system.

## Supported compilers

Currently supported compilers include GCC 4.9+ and Clang 3.5+, both with libstdc++
and libc++.

CI status:

 * Clang: <a href="http://ci.reaver-project.org/viewType.html?buildTypeId=mayfly_TestBuildWithClang&guest=1">
    <img src="http://ci.reaver-project.org/app/rest/builds/buildType:(id:mayfly_TestBuildWithClang)/statusIcon"></a>
 * GCC: <a href="http://ci.reaver-project.org/viewType.html?buildTypeId=mayfly_TestBuildWithGcc&guest=1">
    <img src="http://ci.reaver-project.org/app/rest/builds/buildType:(id:mayfly_TestBuildWithGcc)/statusIcon"></a>

