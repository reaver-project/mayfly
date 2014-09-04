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

#include <boost/algorithm/string.hpp>

#include "testcase.h"
#include "suite.h"
#include "reporter.h"

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        class teamcity_reporter : public reporter
        {
        public:
            virtual void suite_started(const suite & s) const override
            {
                reaver::logger::dlog() << "##teamcity[testSuiteStarted name='" << s.name() << "']";
            }

            virtual void suite_finished(const suite & s) const override
            {
                reaver::logger::dlog() << "##teamcity[testSuiteFinished name='" << s.name() << "']";
            }

            virtual void test_started(const testcase & t) const override
            {
                reaver::logger::dlog() << "##teamcity[testStarted name='" << t.name() << "' captureStandardOutput='true']";
            }

            virtual void test_finished(const testcase_result & result) const override
            {
                auto & name = result.name;
                auto description = result.description;
                boost::algorithm::replace_all(description, "'", "|'");

                switch (result.status)
                {
                    case testcase_status::passed:
                        break;

                    case testcase_status::failed:
                        reaver::logger::dlog() << "##teamcity[testFailed name='" << name << "' details='Test failed: " << description << "']";
                        break;

                    case testcase_status::crashed:
                        reaver::logger::dlog() << "##teamcity[testFailed name='" << name << "' details='Test crashed.']";
                        break;

                    case testcase_status::timed_out:
                        reaver::logger::dlog() << "##teamcity[testFailed name='" << name << "' details='Test timed out.']";
                        break;

                    default:
                        throw invalid_testcase_status{};
                }

                reaver::logger::dlog() << "##teamcity[testFinished name='" << name << "' duration='" << result.duration.count() << "']";
            }

            virtual void summary(const std::vector<std::pair<testcase_status, std::string>> & summary, std::uintmax_t passed, std::uintmax_t total) const override
            {
            }
        };

        MAYFLY_REPORTER_REGISTER("teamcity", teamcity_reporter)
    }}
}
