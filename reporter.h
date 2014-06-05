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
#include <vector>
#include <mutex>
#include <memory>
#include <unordered_map>

#include <reaver/exception.h>

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        class suite;
        class testcase;
        struct testcase_result;

        enum class testcase_status;

        class reporter
        {
        public:
            virtual void suite_started(const suite &) const = 0;
            virtual void suite_finished(const suite &) const = 0;
            virtual void test_started(const testcase &) const = 0;
            virtual void test_finished(const testcase_result &) const = 0;

            virtual void summary(const std::vector<std::pair<testcase_status, std::string>> &, std::uintmax_t passed, std::uintmax_t total) const = 0;

            void lock() const
            {
                _mutex.lock();
            }

            void unlock() const
            {
                _mutex.unlock();
            }

        private:
            mutable std::mutex _mutex;
        };

        class combining_reporter : public reporter
        {
        public:
            combining_reporter(std::vector<std::reference_wrapper<const reporter>> reporters) : _reporters{ std::move(reporters) }
            {
            }

            virtual void suite_started(const suite & s) const override
            {
                for (const auto & r : _reporters)
                {
                    r.get().suite_started(s);
                }
            }

            virtual void suite_finished(const suite & s) const override
            {
                for (const auto & r :  _reporters)
                {
                    r.get().suite_finished(s);
                }
            }

            virtual void test_started(const testcase & t) const override
            {
                for (const auto & r : _reporters)
                {
                    r.get().test_started(t);
                }
            }

            virtual void test_finished(const testcase_result & t) const override
            {
                for (const auto & r : _reporters)
                {
                    r.get().test_finished(t);
                }
            }

            virtual void summary(const std::vector<std::pair<testcase_status, std::string>> & summary, std::uintmax_t passed, std::uintmax_t total) const override
            {
                for (const auto & r : _reporters)
                {
                    r.get().summary(summary, passed, total);
                }
            }

        private:
            std::vector<std::reference_wrapper<const reporter>> _reporters;
        };

        template<typename... Ts>
        combining_reporter combine(const Ts &... reporters)
        {
            return { std::cref(reporters)... };
        }

        combining_reporter combine(const std::vector<std::unique_ptr<reporter>> & reporters)
        {
            std::vector<std::reference_wrapper<const reporter>> tmp;
            tmp.reserve(reporters.size());

            for (const auto & r : reporters)
            {
                tmp.push_back(std::cref(*r));
            }

            return { std::move(tmp) };
        }

        inline std::unordered_map<std::string, std::unique_ptr<reporter>> & reporter_registry()
        {
            static std::unordered_map<std::string, std::unique_ptr<reporter>> registry;
            return registry;
        }

        struct reporter_registrar
        {
            reporter_registrar(std::string name, reporter * registrant)
            {
                reporter_registry().emplace(std::move(name), std::unique_ptr<reporter>{ registrant });
            }
        };
    }}
}

#define MAYFLY_DETAIL_UNIQUE_NAME_CAT(file, line) _mayfly_ ## file ## line
#define MAYFLY_DETAIL_UNIQUE_NAME MAYFLY_DETAIL_UNIQUE_NAME_CAT(__FILE__, __LINE__)

#define MAYFLY_REPORTER_REGISTER(name, ...)                                                                  \
    namespace { static ::reaver::mayfly::reporter_registrar MAYFLY_DETAIL_UNIQUE_NAME { name, new __VA_ARGS__() }; }
