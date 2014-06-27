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

#include <reaver/mayfly/main.h>
#include <reaver/mayfly/teamcity.h>

MAYFLY_BEGIN_SUITE("basic tests");

MAYFLY_ADD_TESTCASE("basic assertions", []
{
    MAYFLY_CHECK(0 == 0);
    MAYFLY_REQUIRE(0 == 0);
    MAYFLY_REQUIRE(!(0 == 1));
    MAYFLY_CHECK(100 == 20 + 80);
});

MAYFLY_ADD_TESTCASE("exceptions-related assertions", []
{
    MAYFLY_CHECK_THROWS(throw 1);
    MAYFLY_CHECK_THROWS_TYPE(int, throw 1);
    MAYFLY_REQUIRE_NOTHROW((void)0);
})

MAYFLY_END_SUITE;
