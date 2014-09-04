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

MAYFLY_BEGIN_SUITE("registration");
MAYFLY_BEGIN_SUITE("registration of tests and suites");

MAYFLY_ADD_TESTCASE("top-level suite", []
{
    reaver::mayfly::suite_registry registry;
    const std::vector<reaver::mayfly::suite> & suites = registry;

    registry.add({ "foobar" });
    MAYFLY_REQUIRE(suites.size() == 1);
    MAYFLY_REQUIRE(suites[0].name() == "foobar");
});

MAYFLY_ADD_TESTCASE("duplicate suite", []
{
    reaver::mayfly::suite_registry registry;
    const std::vector<reaver::mayfly::suite> & suites = registry;

    registry.add({ "foobar" });
    registry.add({ "foobar" });
    MAYFLY_REQUIRE(suites.size() == 1);
    MAYFLY_REQUIRE(suites[0].name() == "foobar");
});

MAYFLY_ADD_TESTCASE("nested suite", []
{
    reaver::mayfly::suite_registry registry;
    const std::vector<reaver::mayfly::suite> & suites = registry;

    registry.add({ "foobar" });
    MAYFLY_REQUIRE_NOTHROW(registry.add({ "foobaz" }, "foobar"));
    MAYFLY_REQUIRE_NOTHROW(registry.add({ "fizzbuzz" }, "foobar/foobaz"));
    MAYFLY_REQUIRE(suites.size() == 1);
    MAYFLY_REQUIRE(suites[0].name() == "foobar");

    const auto & top_level = suites[0];
    const auto & mid_level = top_level.suites()[0];

    MAYFLY_REQUIRE(top_level.suites().size() == 1);
    MAYFLY_REQUIRE_NOTHROW(top_level["foobaz"]);
    MAYFLY_REQUIRE(top_level.suites()[0].name() == "foobaz");
    MAYFLY_REQUIRE(mid_level.suites().size() == 1);
    MAYFLY_REQUIRE_NOTHROW(mid_level["fizzbuzz"]);
    MAYFLY_REQUIRE(mid_level.suites()[0].name() == "fizzbuzz");
});

MAYFLY_ADD_TESTCASE("nested suite without existing parent", []
{
    reaver::mayfly::suite_registry registry;
    MAYFLY_REQUIRE_THROWS_TYPE(reaver::mayfly::unknown_parent, registry.add({ "foobaz" }, "foobar"));
});

MAYFLY_ADD_TESTCASE("test case", []
{
    reaver::mayfly::suite_registry registry;
    const std::vector<reaver::mayfly::suite> & suites = registry;

    registry.add({ "foobar"});
    MAYFLY_REQUIRE_NOTHROW(registry.add("foobar", { "foobaz", []{} }));
    MAYFLY_REQUIRE(suites[0].begin()->name() == "foobaz");

    registry.add({ "fizzbuzz" }, "foobar");
    MAYFLY_REQUIRE_NOTHROW(registry.add("foobar/fizzbuzz", { "barfoo", []{} }));
    MAYFLY_REQUIRE(suites[0].suites()[0].begin()->name() == "barfoo");
});

MAYFLY_ADD_TESTCASE("test case without existing parent", []
{
    reaver::mayfly::suite_registry registry;
    MAYFLY_REQUIRE_THROWS_TYPE(reaver::mayfly::unknown_suite, registry.add("foobar", { "barfoo", []{} }));
});

MAYFLY_END_SUITE;
MAYFLY_END_SUITE;
