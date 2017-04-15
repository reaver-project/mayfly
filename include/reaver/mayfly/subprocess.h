/**
 * Mayfly License
 *
 * Copyright © 2014-2015 Michał "Griwes" Dominiak
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

#include <boost/algorithm/string.hpp>

#include "testcase.h"
#include "suite.h"
#include "reporter.h"
#include "detail/atexit.h"

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        class subprocess_reporter : public reporter
        {
        public:
            virtual void suite_started(const suite &) const override
            {
            }

            virtual void suite_finished(const suite &) const override
            {
            }

            virtual void test_started(const testcase &) const override
            {
                _detail::_default_atexit_registry().atexit(_atexit);

                std::cout << "{{started}}\n";
            }

            virtual void test_finished(const testcase_result & result) const override
            {
                switch (result.status)
                {
                    case testcase_status::passed:
                        break;

                    case testcase_status::failed:
                        std::cout << "{{failed " << result.description << "}}\n";
                        break;

                    default:
                        std::cout << "{{error unexpected test status}}\n";
                        break;
                }

                std::cout << "{{finished}}\n";
            }

            virtual void summary(const tests_summary & summary) const override
            {
                if (!summary.total)
                {
                    std::cout << "{{error not found}}\n";
                }
            }

        private:
            static void _atexit()
            {
                std::cout << "{{exit}}\n";
            }
        };

        MAYFLY_REPORTER_REGISTER("subprocess", subprocess_reporter)
    }}
}
