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

#include <iomanip>

#include <boost/algorithm/string.hpp>

#include "reporter.h"
#include "suite.h"

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        class console_reporter : public reporter
        {
        public:
            virtual void suite_started(const suite & s) const override
            {
                reaver::logger::dlog(reaver::logger::info) << "entering suite `" << s.name() << "`.";
            }

            virtual void suite_finished(const suite & s) const override
            {
                reaver::logger::dlog(reaver::logger::info) << "leaving suite `" << s.name() << "`.\n";
            }

            virtual void test_started(const testcase & t) const override
            {
                reaver::logger::dlog(reaver::logger::info) << "executing test `" << t.name() << "`.";
            }

            virtual void test_finished(const testcase_result & result) const override
            {
                std::string description = result.description;
                boost::algorithm::replace_all(description, "|n", "\n");

                switch (result.status)
                {
                    case testcase_status::passed:
                        reaver::logger::dlog(reaver::logger::success) << "test passed: `" << result.name << "`, in " << result.duration.count() << "ms.";
                        break;

                    case testcase_status::failed:
                        reaver::logger::dlog(reaver::logger::error) << "test failed: `" << result.name << "`, in " << result.duration.count() << "ms." << (description.empty() ? "" : "\nReason: ")
                            << style::style() << description;
                        break;

                    case testcase_status::crashed:
                        reaver::logger::dlog(reaver::logger::error) << "test crashed: `" << result.name << "`, in " << result.duration.count() << "ms." << (description.empty() ? "" : "\nReason: ")
                            << style::style() << description;
                        break;

                    case testcase_status::timed_out:
                        reaver::logger::dlog(reaver::logger::error) << "test timed out: `" << result.name << "`.";
                        break;

                    default:
                        throw invalid_testcase_status{};
                }
            }

            virtual void summary(tests_summary summary) const override
            {
                auto && white = style::style(style::colors::bgray, style::colors::def, style::styles::bold);
                auto && red = style::style(style::colors::bred, style::colors::def, style::styles::bold);
                auto && green = style::style(style::colors::green, style::colors::def, style::styles::bold);
                auto && yellow = style::style(style::colors::bbrown, style::colors::def, style::styles::bold);

                std::uintmax_t crashed = 0;
                std::uintmax_t timed_out = 0;

                if (summary.failed_tests.size())
                {
                    reaver::logger::dlog() << white << "\nSummary:";
                }

                for (const auto & elem : summary.failed_tests)
                {
                    switch (elem.first)
                    {
                        case testcase_status::failed:
                            reaver::logger::dlog() << white << " - " << elem.second << ": " << red << "FAILED";
                            break;

                        case testcase_status::crashed:
                            reaver::logger::dlog() << white << " - " << elem.second << ": " << red << "CRASHED";
                            ++crashed;
                            break;

                        case testcase_status::timed_out:
                            reaver::logger::dlog() << white << " - " << elem.second << ": " << yellow << "TIMED OUT";
                            ++timed_out;
                            break;

                        default:
                            ;
                    }
                }

                if (summary.failed_tests.size())
                {
                    reaver::logger::dlog();
                }

                if (!summary.total)
                {
                    reaver::logger::dlog() << white << "No tests found.";
                    return;
                }

                if (summary.passed == summary.total)
                {
                    reaver::logger::dlog() << green << "All tests passed!";
                }

                auto width = std::to_string(summary.total).size();

                if (summary.passed)
                {
                    reaver::logger::dlog() << green << "Passed" <<  white << ":    " << to_string_width(summary.passed, width) << " / " << summary.total;
                }

                if (summary.total - summary.passed - crashed - timed_out)
                {
                    reaver::logger::dlog() << red << "Failed" <<  white << ":    " << to_string_width(summary.total - summary.passed - crashed - timed_out, width) << " / " << summary.total;
                }

                if (crashed)
                {
                    reaver::logger::dlog() << red << "Crashed" <<  white << ":   " << to_string_width(crashed, width) << " / " << summary.total;
                }

                if (timed_out)
                {
                    reaver::logger::dlog() << yellow << "Timed out" << white << ": " << to_string_width(timed_out, width) << " / " << summary.total;
                }

                if (summary.actual_time.count())
                {
                    reaver::logger::dlog() << green << "Clock time taken" << white << ": " << summary.actual_time.count() << "ms.";
                }
            }
        };

        MAYFLY_REPORTER_REGISTER("console", console_reporter)
    }}
}
