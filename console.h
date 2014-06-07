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

#include "reporter.h"
#include "suite.h"

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        class invalid_testcase_status : public exception
        {
        public:
            invalid_testcase_status() : exception{ reaver::logger::crash }
            {
                *this << "a test result contained invalid state.";
            }
        };

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
                switch (result.status)
                {
                    case testcase_status::passed:
                        reaver::logger::dlog(reaver::logger::success) << "test passed: `" << result.name << "`.";
                        break;

                    case testcase_status::failed:
                        reaver::logger::dlog(reaver::logger::error) << "test failed: `" << result.name << "`.\n" << "Reason: " << style::style() << result.description;
                        break;

                    case testcase_status::crashed:
                        reaver::logger::dlog(reaver::logger::error) << "test crashed: `" << result.name << "`.\n" << "Reason: " << style::style() << result.description;
                        break;

                    default:
                        throw invalid_testcase_status{};
                }
            }

            virtual void summary(const std::vector<std::pair<testcase_status, std::string>> & summary, std::uintmax_t passed, std::uintmax_t total) const override
            {
                auto && white = style::style(style::colors::bgray, style::colors::def, style::styles::bold);
                auto && red = style::style(style::colors::bred, style::colors::def, style::styles::bold);
                auto && green = style::style(style::colors::green, style::colors::def, style::styles::bold);

                std::uintmax_t crashed = 0;

                if (summary.size())
                {
                    reaver::logger::dlog() << white << "\nSummary:";
                }

                for (const auto & elem : summary)
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

                        default:
                            ;
                    }
                }

                if (summary.size())
                {
                    reaver::logger::dlog();
                }

                if (!total)
                {
                    reaver::logger::dlog() << white << "No tests found.";
                    return;
                }

                if (passed == total)
                {
                    reaver::logger::dlog() << green << "All tests passed!";
                }

                if (passed)
                {
                    reaver::logger::dlog() << green << "Passed" <<  white << ":  " << passed << " / " << total;
                }

                if (total - passed - crashed)
                {
                    reaver::logger::dlog() << red << "Failed" <<  white << ":  " << (total - passed - crashed) << " / " << total;
                }

                if (crashed)
                {
                    reaver::logger::dlog() << red << "Crashed" <<  white << ": " << crashed << " / " << total;
                }
            }
        };

        MAYFLY_REPORTER_REGISTER("console", console_reporter)
    }}
}
