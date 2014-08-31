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

#pragma once

#include <string>
#include <functional>
#include <chrono>

#include "asserts.h"

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        enum class testcase_status
        {
            not_started = 0,
            passed = 1,
            failed = 2,
            crashed = 3,
            timed_out = 4,
            not_found = 5
        };

        struct testcase_result
        {
            std::string name;
            testcase_status status;
            std::string description;
            std::chrono::milliseconds duration;
        };

        class testcase
        {
        public:
            testcase(std::string name, std::function<void ()> test, bool positive = true, std::size_t assertions_to_fail = 0) : _name{ std::move(name) }, _test{ std::move(test) },
                _positive{ positive }, _assertions_to_fail{ assertions_to_fail }
            {
            }

            const std::string & name() const
            {
                return _name;
            }

            void operator()() const
            {
                try
                {
                    _detail::_local_assertions_logger() = _detail::_assertions_logger{ _positive, _assertions_to_fail };
                    _test();
                    _detail::_local_assertions_logger()->throw_exception();
                    _detail::_local_assertions_logger() = {};
                }

                catch (expected_failure_exit &)
                {
                }
            }

        private:
            std::string _name;
            std::function<void ()> _test;
            bool _positive;
            std::size_t _assertions_to_fail;
        };
    }}
}
