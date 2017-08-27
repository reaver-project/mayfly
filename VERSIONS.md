# Planned and past releases

## Release #1

* 0.0.x dev - early development; not versioned
* 0.1.x:
 * 0.1.0 - initial release; console reporter, subprocess runner and basic checking macros done at least partially - *finished on 09.06.2014*
 * 0.1.1 - nested suites; negative tests; TeamCity reporter; duration measurement for the tests - *finished on 06.09.2014*
 * 0.1.2 - refactor subprocess runner to allow printing (almost?) arbitrary output from tests; print files and lines in assertions - *finished on 02.10.2014*
 * 0.1.3 - align to an officially released Boost.Process - *finished on 27.08.2017*
 * 0.1.4 - print values of operands in `MAYFLY_CHECK` and `MAYFLY_REQUIRE`; print info from exceptions thrown in tests
 * 0.1.5 - test tags
 * 0.1.6 - add support for sanitizers: leak, address, memory, thread, ub; augment reporters with a function to log standard output from tests in a custom way
* 0.2.x:
 * 0.2.0 - support for fixtures
