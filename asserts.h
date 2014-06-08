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

#include <stdexcept>
#include <vector>
#include <unordered_map>

#include <boost/optional.hpp>

#include <reaver/exception.h>

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        class assertions_failed : public std::runtime_error
        {
        public:
            assertions_failed(std::string description) : std::runtime_error{ std::move(description) }
            {
            }
        };

        namespace _detail
        {
            class _assertions_logger
            {
            public:
                void log(std::string str, bool critical = false)
                {
                    _assertions.push_back(std::move(str));

                    if (critical)
                    {
                        throw_exception();
                    }
                }

                void throw_exception()
                {
                    if (_assertions.empty())
                    {
                        return;
                    }

                    bool single = _assertions.size() == 1;

                    std::string exception_string = "assertion";
                    exception_string += single ? "" : "s";
                    exception_string += " failed:";

                    if (single)
                    {
                        exception_string += _assertions[0];
                    }

                    else
                    {
                        for (auto && e : _assertions)
                        {
                            exception_string += "|n - " + e;
                        }
                    }

                    _assertions.clear();
                    throw assertions_failed{ std::move(exception_string) };
                }

            private:
                std::vector<std::string> _assertions;
            };

            boost::optional<_assertions_logger> & _local_assertions_logger()
            {
                static std::unordered_map<std::thread::id, boost::optional<_assertions_logger>> loggers;

                return loggers[std::this_thread::get_id()];
            }
        }

        class invalid_log_assertion_call : public exception
        {
        public:
            invalid_log_assertion_call() : exception{ logger::crash }
            {
                *this << "mayfly::log_assertion() called outside of a testcase.";
            }
        };

        void log_assertion(std::string description, bool critical = false)
        {
            auto & logger = _detail::_local_assertions_logger();

            if (!logger)
            {
                throw invalid_log_assertion_call{};
            }

            logger->log(std::move(description), critical);
        }
    }}
}

#define MAYFLY_REQUIRE(...) \
    if (!(__VA_ARGS__)) { ::reaver::mayfly::log_assertion(#__VA_ARGS__, true); }

#define MAYFLY_CHECK(...) \
    if (!(__VA_ARGS__)) { ::reaver::mayfly::log_assertion(#__VA_ARGS__); }
