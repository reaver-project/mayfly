# Mayfly

Mayfly is the Reaver Project's testing framework. It's meant to be lightweight and
benefit from as many modern C++ features as possible. Primary goals are for Mayfly
to be fast, generate readable outputs (and TeamCity-ready results) and later get
closely integrated with Despayre, the Reaver Project's build system.

# Tutorial: basic assertions

Let's write a simple testing binary with two test suites and some test cases inside.
There are basically two ways of including Mayfly - either using `#include <reaver/mayfly.h>`,
which will include the framework, but not prepare a `main()` for you, and `#include <reaver/mayfly/main.h>`,
which will do both. Let's use the easier option, since chances are that the default
`main()` will usually be enough.

    #include <reaver/mayfly/main.h>

Ok, now what? We need to add some tests to be compiled and executed by the binary.
Let's call the suite `basic tests`.

    MAYFLY_BEGIN_SUITE("basic tests");
    MAYFLY_END_SUITE;

Notice the second line - always remember to close a suite after opening it. Also
note that while nested suites will compile, they will not create a hierarchy of
suites.

Now, what use is a test suite without tests? Let's add two simple test cases which
will deal with addition and subtraction:

    MAYFLY_BEGIN_SUITE("basic tests");

    MAYFLY_ADD_TESTCASE("addition", []
    {
        MAYFLY_CHECK(1 + 1 == 2);
        MAYFLY_CHECK(5 + 7 == 7 + 5);
        MAYFLY_CHECK(1 + 0 == 1);
    });

    MAYFLY_ADD_TESTCASE("subtraction", []
    {
        MAYFLY_REQUIRE(1 - 1 == 0);
        MAYFLY_REQUIRE(5 - 7 == -(7 - 5));
        MAYFLY_REQUIRE(1 - 0 == 1);
    });

    MAYFLY_END_SUITE;

You can see that those test cases use different macros to test the assertions -
I will talk about those in a while.

If you will now compile and execute these tests(remember to link in all the
necessary libraries: `-lboost_system -lboost_iostreams -lboost_program_options
-lreaver -pthread`), you will see the following:

    Info: entering suite `basic tests`.
    Info: executing test `addition`.
    Success: test passed: `addition`.
    Info: executing test `subtraction`.
    Success: test passed: `subtraction`.
    Info: leaving suite `basic tests`.

    All tests passed!
    Passed:  2 / 2

Yay! All tests passed; that's good. But now, the main reason we want to use a
testing framework is to detect actual errors, so let's break the tests. The
easiest way to do that would be to replace all `==` with `!=`, right? Let's do
that. And now, when you do that, compile and run the binary, you will see the
following output:

    Info: entering suite `basic tests`.
    Info: executing test `addition`.
    Error: test failed: `addition`.
    Reason: assertions failed:
     - 1 + 1 != 2
     - 5 + 7 != 7 + 5
     - 1 + 0 != 1
    Info: executing test `subtraction`.
    Error: test failed: `subtraction`.
    Reason: assertion failed: 1 - 1 != 0
    Info: leaving suite `basic tests`.


    Summary:
     - basic tests/addition: FAILED
     - basic tests/subtraction: FAILED

    Failed:  2 / 2

We can see that both test cases failed; that's good, that's what we wanted. Now,
the difference between `MAYFLY_CHECK` and `MAYFLY_REQUIRE` is clearly visible.
`MAYFLY_REQUIRE` immediately interrupts the execution if failed, so the rest of
the test case doesn't run. `MAYFLY_CHECK`, on the other hand, allows other test
cases to run after it has failed. What if they are mixed? The same thing happens,
and if any `MAYFLY_CHECK` has failed before a `MAYFLY_REQUIRE` fails, their reasons
are also added to the failure reason, just like above.

# Tutorial: testing exceptions

TODO
