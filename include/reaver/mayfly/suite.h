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
#include <vector>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include "testcase.h"
#include "reporter.h"

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        class suite
        {
        public:
            suite(std::string name, std::vector<testcase> testcases = {}) : _name{ std::move(name) }, _testcases{ std::move(testcases) }
            {
            }

            void add(testcase t)
            {
                _testcases.push_back(std::move(t));
            }

            void add(std::string name, std::function<void ()> testcase)
            {
                add({ std::move(name), std::move(testcase) });
            }

            const std::string & name() const
            {
                return _name;
            }

            auto begin() const
            {
                return _testcases.begin();
            }

            auto end() const
            {
                return _testcases.end();
            }

            auto cbegin() const
            {
                return _testcases.cbegin();
            }

            auto cend() const
            {
                return _testcases.cend();
            }

        private:
            std::string _name;
            std::vector<testcase> _testcases;
        };

        class duplicate_suite_registration : public exception
        {
        public:
            duplicate_suite_registration(const std::string & suite_name) : exception{ reaver::logger::error }
            {
                *this << "tried to register a duplicate suite `" << suite_name << "`.";
            }
        };

        class duplicate_testcase_registration : public exception
        {
        public:
            duplicate_testcase_registration(const std::string & suite_name, const std::string & testcase_name) : exception{ reaver::logger::error }
            {
                *this << "tried to register a duplicate testcase `" << suite_name << '/' << testcase_name << "`.";
            }
        };

        class unknown_suite : public exception
        {
        public:
            unknown_suite(const std::string & suite_name, const std::string & testcase_name) : exception{ reaver::logger::error }
            {
                *this << "tried to register a test `" << testcase_name << "` for an unknown suite `" << suite_name << "`.";
            }
        };

        class suite_registry
        {
        public:
            operator const std::vector<suite> &() const
            {
                return _suites;
            }

            void add(suite s)
            {
                if (_names.find(s.name()) != _names.end())
                {
                    throw duplicate_suite_registration{ s.name() };
                }

                _names.emplace(s.name(), _suites.size());
                _suites.push_back(s);
            }

            void add(const std::string & suite_name, testcase t)
            {
                if (_names.find(suite_name) == _names.end())
                {
                    throw unknown_suite{ suite_name, t.name() };
                }

                if (_testcases[suite_name].find(t.name()) != _testcases[suite_name].end())
                {
                    throw duplicate_testcase_registration{ suite_name, t.name() };
                }

                _testcases[suite_name].emplace(t.name());
                _suites[_names[suite_name]].add(t);
            }

        private:
            std::vector<suite> _suites;
            std::unordered_map<std::string, std::size_t> _names;
            std::unordered_map<std::string, std::unordered_set<std::string>> _testcases;
        };

        inline suite_registry & default_suite_registry()
        {
            static suite_registry default_registry;

            return default_registry;
        };

        struct suite_registrar
        {
            suite_registrar(suite s)
            {
                try
                {
                    default_suite_registry().add(std::move(s));
                }

                catch (reaver::exception & e)
                {
                    e.print(reaver::logger::default_logger());
                    std::exit(2);
                }

                catch (std::exception & e)
                {
                    reaver::logger::dlog(reaver::logger::crash) << e.what();
                    std::exit(2);
                }
            }
        };

        struct testcase_registrar
        {
            testcase_registrar(const std::string & suite_name, testcase t)
            {
                try
                {
                    default_suite_registry().add(suite_name, std::move(t));
                }

                catch (reaver::exception & e)
                {
                    e.print(reaver::logger::default_logger());
                    std::exit(2);
                }

                catch (std::exception & e)
                {
                    reaver::logger::dlog(reaver::logger::crash) << e.what();
                    std::exit(2);
                }
            }
        };
    }}
}

#define MAYFLY_ADD_SUITE(name) \
    namespace { static ::reaver::mayfly::suite_registrar MAYFLY_DETAIL_UNIQUE_NAME { ::reaver::mayfly::suite { name } }; }

#define MAYFLY_ADD_TESTCASE_TO(suite, test, ...)                                                                            \
    namespace { static ::reaver::mayfly::testcase_registrar MAYFLY_DETAIL_UNIQUE_NAME { suite, ::reaver::mayfly::testcase { \
        test, __VA_ARGS__ } }; }

#define MAYFLY_BEGIN_SUITE(name)                                                           \
    MAYFLY_ADD_SUITE(name)                                                                 \
    namespace { namespace MAYFLY_DETAIL_UNIQUE_NAME { const std::string && _suite_name = name;

#define MAYFLY_CONTINUE_SUITE(name)                                                        \
    namespace { namespace MAYFLY_DETAIL_UNIQUE_NAME { const std::string && _suite_name = name;

#define MAYFLY_END_SUITE \
    } }

#define MAYFLY_ADD_TESTCASE(test, ...) \
    MAYFLY_ADD_TESTCASE_TO(_suite_name, test, __VA_ARGS__)

#define MAYFLY_ADD_NEGATIVE_TESTCASE_TO(suite, test, ...) \
    namespace { static ::reaver::mayfly::testcase_registrar MAYFLY_DETAIL_UNIQUE_NAME { suite, ::reaver::mayfly::testcase { \
        test, __VA_ARGS__, false } }; }

#define MAYFLY_ADD_NEGATIVE_TESTCASE(test, ...) \
    MAYFLY_ADD_NEGATIVE_TESTCASE_TO(_suite_name, test, __VA_ARGS__)

#define MAYFLY_ADD_NEGATIVE_TESTCASE_N_TO(suite, test, N, ...) \
    namespace { static ::reaver::mayfly::testcase_registrar MAYFLY_DETAIL_UNIQUE_NAME { suite, ::reaver::mayfly::testcase { \
        test, __VA_ARGS__, false, N } }; }

#define MAYFLY_ADD_NEGATIVE_TESTCASE_N(test, N, ...) \
    MAYFLY_ADD_NEGATIVE_TESTCASE_N_TO(_suite_name, test, N, __VA_ARGS__)
