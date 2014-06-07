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

#include <memory>
#include <iostream>

#include <boost/process.hpp>
#include <boost/process/initializers.hpp>
#include <boost/program_options.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <reaver/thread_pool.h>

#include "reporter.h"
#include "testcase.h"
#include "suite.h"
#include "console.h"

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        class runner
        {
        public:
            runner(std::size_t threads = 1, std::string suite_name = "", std::string test_name = "") : _threads{ threads }, _suite_name{ std::move(suite_name) },
                _test_name{ std::move(test_name) }
            {
            }

            virtual void operator()(const std::vector<suite> & suites, const reporter & rep) = 0;

            std::size_t total() const
            {
                return _tests;
            }

            std::size_t passed() const
            {
                return _passed;
            }

            auto summary(const reporter & rep) const
            {
                rep.summary(_failed, _passed, _tests);
            }

        protected:
            std::size_t _threads = 1;
            std::size_t _limit = 0;

            std::string _suite_name;
            std::string _test_name;

            std::atomic<std::uintmax_t> _tests{};
            std::atomic<std::uintmax_t> _passed{};

            std::vector<std::pair<testcase_status, std::string>> _failed;
        };

        class subprocess_runner : public runner
        {
        public:
            subprocess_runner(std::string executable, std::size_t threads = 1, std::string suite = "", std::string test_name = "") : runner{ threads, std::move(suite),
                std::move(test_name) }, _executable{ std::move(executable) }
            {
            }

            virtual void operator()(const std::vector<suite> & suites, const reporter & rep) override
            {
                if (!_test_name.empty())
                {
                    for (const auto & s : suites)
                    {
                        if (s.name() != _suite_name)
                        {
                            continue;
                        }

                        for (const auto & test : s)
                        {
                            if (test.name() == _test_name)
                            {
                                auto result = _run_test(test, s, rep);
                                std::cout << static_cast<std::uintmax_t>(result.status) << " " << result.description << std::flush;

                                ++_tests;

                                if (result.status == testcase_status::passed)
                                {
                                    ++_passed;
                                }

                                return;
                            }
                        }
                    }

                    std::cout << static_cast<std::uintmax_t>(testcase_status::not_found) << std::flush;
                    return;
                }

                for (const auto & s : suites)
                {
                    rep.suite_started(s);

                    {
                        thread_pool pool(_threads);

                        for (const auto & test : s)
                        {
                            ++_tests;

                            pool.push([&]()
                            {
                                auto result = _run_test(test, s, rep);

                                if (result.status == testcase_status::passed)
                                {
                                    ++_passed;
                                }

                                else
                                {
                                    _failed.push_back(std::make_pair(result.status, s.name() + "/" + test.name()));
                                }
                            });
                        }
                    }

                    rep.suite_finished(s);
                }
            }

        private:
            std::string _executable;

            testcase_result _run_test(const testcase & t, const suite & s, const reporter & rep) const
            {
                testcase_result result;
                result.name = t.name();

                if (_threads == 1)
                {
                    rep.test_started(t);
                }

                if (t.name() == _test_name)
                {
                    try
                    {
                        t();
                        result.status = testcase_status::passed;
                    }

                    catch (reaver::exception & e)
                    {
                        std::ostringstream str;

                        {
                            reaver::logger::logger l{};
                            l.add_stream(str);
                            e.print(l);
                        }

                        result.status = testcase_status::failed;
                        result.description = str.str();
                    }

                    catch (std::exception & e)
                    {
                        result.status = testcase_status::failed;
                        result.description = e.what();
                    }
                }

                else
                {
                    using namespace boost::process::initializers;

                    std::vector<std::string> args{ _executable, "--test", s.name() + "/" + t.name(), "-q" };

                    boost::process::pipe p = boost::process::create_pipe();

                    {
                        boost::iostreams::file_descriptor_sink sink{ p.sink, boost::iostreams::close_handle };

                        boost::process::execute(set_args(args), bind_stdout(sink), close_stdin());
                    }

                    boost::iostreams::file_descriptor_source source{ p.source, boost::iostreams::close_handle };
                    boost::iostreams::stream<boost::iostreams::file_descriptor_source> is(source);

                    std::uintmax_t retval;
                    std::string message;

                    is >> retval;
                    std::getline(is, message);

                    if (!is || retval > 3)
                    {
                        result.status = testcase_status::crashed;
                    }

                    else
                    {
                        result.status = static_cast<testcase_status>(retval);
                        result.description = message;
                    }
                }

                if (_threads != 1)
                {
                    std::unique_lock<const reporter> lock(rep);
                    rep.test_started(t);
                    rep.test_finished(result);
                }

                else
                {
                    rep.test_finished(result);
                }

                return result;
            }
        };

        class invalid_default_runner_initialization : public exception
        {
        public:
            invalid_default_runner_initialization() : exception{ logger::crash }
            {
                *this << "attempted to initialize Mayfly's default runner with a null value.";
            }
        };

        inline runner & default_runner(std::unique_ptr<runner> new_default = nullptr)
        {
            static std::unique_ptr<runner> default_runner = [&]()
            {
                if (new_default)
                {
                    return std::move(new_default);
                }

                throw invalid_default_runner_initialization{};
            }();

            if (new_default)
            {
                default_runner = std::move(new_default);
            }

            return *default_runner;
        }

        constexpr const char * version_string = "Reaver Project's Mayfly v0.0.1 dev\nCopyright © 2014 Reaver Project Team\n";

        class invalid_testcase_name_format : public exception
        {
        public:
            invalid_testcase_name_format(const std::string & test_name) : exception{ reaver::logger::error }
            {
                *this << "invalid testcase name format - proper format is `suite/testcase`.";
            }
        };

        inline int run(const std::vector<suite> & suites, int argc, char ** argv)
        {
            std::size_t threads = 1;
            std::string test_name;
            std::vector<std::string> reporters;
            std::string executable = argv[0];

            boost::program_options::variables_map variables;

            boost::program_options::options_description general("General");
            general.add_options()
                ("help,h", "print this message")
                ("version,v", "print version information");

            boost::program_options::options_description config("Configuration");
            config.add_options()
                ("j", boost::program_options::value<std::size_t>(&threads)->default_value(1), "specify the amount of worker threads")
                ("test,t", boost::program_options::value<std::string>(), "specify the thread to run")
                ("reporter,r", boost::program_options::value<std::vector<std::string>>()->composing(), "select a reporter to use")
                ("quiet,q", "disable reporters")
                ("error,e", "only show errors and summary (controls console output)");

            boost::program_options::options_description options;
            options.add(general).add(config);

            boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options)
                .style(boost::program_options::command_line_style::allow_short
                    | boost::program_options::command_line_style::allow_long
                    | boost::program_options::command_line_style::allow_sticky
                    | boost::program_options::command_line_style::allow_dash_for_short
                    | boost::program_options::command_line_style::long_allow_next
                    | boost::program_options::command_line_style::short_allow_next
                    | boost::program_options::command_line_style::allow_long_disguise).run(), variables);

            if (variables.count("help"))
            {
                std::cout << version_string << '\n';
                std::cout << general << '\n' << config;

                return 0;
            }

            if (variables.count("version"))
            {
                std::cout << version_string;
                std::cout << "Distributed under modified zlib license.\n\n";

                std::cout << "Mayfly is the Reaver Project's free testing framework.\n";

                return 0;
            }

            if (variables.count("test"))
            {
                test_name = variables["test"].as<std::string>();
            }

            if (variables.count("reporter"))
            {
                reporters = variables["reporter"].as<std::vector<std::string>>();
            }

            else if (!variables.count("quiet"))
            {
                reporters.push_back("console");
            }

            if (variables.count("error"))
            {
                reaver::logger::dlog.set_level(reaver::logger::error);
            }

            std::vector<std::reference_wrapper<const reporter>> reps;
            for (const auto & elem : reporters)
            {
                reps.emplace_back(std::cref(*reporter_registry().at(elem)));
            }

            std::string suite;
            if (!test_name.empty() && test_name.find('/') == std::string::npos)
            {
                if (!variables.count("quiet"))
                {
                    throw invalid_testcase_name_format{ test_name };
                }

                else
                {
                    std::cout << static_cast<std::uintmax_t>(testcase_status::not_found) << std::flush;
                    return 1;
                }
            }

            if (!test_name.empty())
            {

                suite = test_name.substr(0, test_name.find('/'));
                test_name = test_name.substr(test_name.find('/') + 1);
            }

            auto && reporter = combine(reps);
            default_runner(std::make_unique<subprocess_runner>(executable, threads, suite, test_name))(suites, reporter);
            default_runner().summary(reporter);

            if (default_runner().passed() == default_runner().total())
            {
                return 0;
            }

            return 1;
        }
    }}
}
