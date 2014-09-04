/**
 * Mayfly License
 *
 * Copyright © 2014 Michał "Griwes" Dominiak
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation is required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 **/

#include "mayfly.h"

namespace
{
    void doesnt_throw()
    {
    }
}

MAYFLY_BEGIN_SUITE("assertions");
MAYFLY_BEGIN_SUITE("exceptions-related assertions");

MAYFLY_ADD_TESTCASE("check throws", []
{
    MAYFLY_CHECK_THROWS(throw 1);
});

MAYFLY_ADD_TESTCASE("check throws type", []
{
    MAYFLY_CHECK_THROWS_TYPE(int, throw 1);
});

MAYFLY_ADD_TESTCASE("check nothrow", []
{
    MAYFLY_CHECK_NOTHROW(doesnt_throw());
});

MAYFLY_ADD_TESTCASE("require throws", []
{
    MAYFLY_REQUIRE_THROWS(throw 1);
});

MAYFLY_ADD_TESTCASE("require throws type", []
{
    MAYFLY_REQUIRE_THROWS_TYPE(int, throw 1);
});

MAYFLY_ADD_TESTCASE("require nothrow", []
{
    MAYFLY_REQUIRE_NOTHROW(doesnt_throw());
});

MAYFLY_ADD_NEGATIVE_TESTCASE("failed check throws", []
{
    MAYFLY_CHECK_THROWS(doesnt_throw());
});

MAYFLY_ADD_NEGATIVE_TESTCASE("failed check throws type", []
{
    MAYFLY_CHECK_THROWS_TYPE(int, doesnt_throw());
});

MAYFLY_ADD_NEGATIVE_TESTCASE("failed check nothrow", []
{
    MAYFLY_CHECK_NOTHROW(throw 1);
});

MAYFLY_ADD_NEGATIVE_TESTCASE("failed require throws", []
{
    MAYFLY_REQUIRE_THROWS(doesnt_throw());
});

MAYFLY_ADD_NEGATIVE_TESTCASE("failed require throws type", []
{
    MAYFLY_REQUIRE_THROWS_TYPE(int, doesnt_throw());
});

MAYFLY_ADD_NEGATIVE_TESTCASE("failed require nothrow", []
{
    MAYFLY_REQUIRE_NOTHROW(throw 1);
});

MAYFLY_ADD_NEGATIVE_TESTCASE("check throws type mismatch", []
{
    MAYFLY_CHECK_THROWS_TYPE(int, throw false);
});

MAYFLY_ADD_NEGATIVE_TESTCASE("require throws type mismatch", []
{
    MAYFLY_REQUIRE_THROWS_TYPE(int, throw false);
});

MAYFLY_END_SUITE;
MAYFLY_END_SUITE;
