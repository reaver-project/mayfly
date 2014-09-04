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
            assertions_failed(std::string description, std::size_t count) : std::runtime_error{ std::move(description) }, _count{ count }
            {
            }

            std::size_t count() const
            {
                return _count;
            }

        private:
            std::size_t _count;
        };

        class expected_failure : public std::runtime_error
        {
        public:
            expected_failure(std::size_t count, std::size_t failed, std::string asserts) : std::runtime_error{ "negative test conditions not met" + (count ? " (" + std::to_string(count) +
                " assertions expected to fail, " : " (") + std::to_string(failed) + " assertions failed); " + asserts }
            {
            }
        };

        class expected_failure_exit
        {
        };

        namespace _detail
        {
            class _assertions_logger
            {
            public:
                _assertions_logger(bool positive = true, std::size_t assertions_to_fail = 0) : _positive{ positive }, _assertions_to_fail{ assertions_to_fail }
                {
                }

                void log(std::string str, bool critical = false)
                {
                    _assertions.push_back(std::move(str));

                    if (critical)
                    {
                        throw_exception(critical);
                    }
                }

                void throw_exception(bool critical = false)
                {
                    if (!_positive)
                    {
                        if (_assertions_to_fail && _assertions.size() != _assertions_to_fail)
                        {
                            throw expected_failure{ _assertions_to_fail, _assertions.size(), _build_exception_string() };
                        }

                        else
                        {
                            throw expected_failure_exit{};
                        }
                    }

                    else if (_assertions.empty())
                    {
                        return;
                    }

                    auto size = _assertions.size();
                    auto exception_string = _build_exception_string();
                    _assertions.clear();
                    throw assertions_failed{ std::move(exception_string), size };
                }

            private:
                std::string _build_exception_string()
                {
                    bool single = _assertions.size() == 1;

                    std::string exception_string = "assertion";
                    exception_string += single ? "" : "s";
                    exception_string += " failed: ";

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

                    return exception_string;
                }

                std::vector<std::string> _assertions;
                bool _positive;
                std::size_t _assertions_to_fail;
            };

            inline boost::optional<_assertions_logger> & _local_assertions_logger()
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

        inline void log_assertion(std::string description, bool critical = false)
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

#define MAYFLY_REQUIRE(...)                                                              \
    try { if (!(__VA_ARGS__)) { ::reaver::mayfly::log_assertion(#__VA_ARGS__, true); } } \
    catch (::reaver::mayfly::expected_failure_exit) { throw; }                           \
    catch (...) { ::reaver::mayfly::log_assertion(#__VA_ARGS__ " has thrown an unexpected exception", true); }

#define MAYFLY_CHECK(...)                                                          \
    try { if (!(__VA_ARGS__)) { ::reaver::mayfly::log_assertion(#__VA_ARGS__); } } \
    catch (::reaver::mayfly::expected_failure_exit) { throw; }                     \
    catch (...) { ::reaver::mayfly::log_assertion(#__VA_ARGS__ " has thrown an unexpected exception", true); }

#define MAYFLY_REQUIRE_THROWS(...) \
    try { __VA_ARGS__; ::reaver::mayfly::log_assertion(#__VA_ARGS__ " should have thrown, but didn't", true); } catch (...) {}

#define MAYFLY_CHECK_THROWS(...) \
    try { __VA_ARGS__; ::reaver::mayfly::log_assertion(#__VA_ARGS__ " should have thrown, but didn't"); } catch (...) {}

#define MAYFLY_REQUIRE_THROWS_TYPE(type, ...)                                                                                            \
    try { __VA_ARGS__; ::reaver::mayfly::log_assertion(#__VA_ARGS__ " should have thrown " #type ", but didn't throw anything", true); } \
    catch (type & e) {}                                                                                                                  \
    catch (...) { ::reaver::mayfly::log_assertion(#__VA_ARGS__ " should have thrown " #type ", but has thrown something else", true); }

#define MAYFLY_CHECK_THROWS_TYPE(type, ...)                                                                                        \
    try { __VA_ARGS__; ::reaver::mayfly::log_assertion(#__VA_ARGS__ " should have thrown " #type ", but didn't throw anything"); } \
    catch (type &) {}                                                                                                              \
    catch (...) { ::reaver::mayfly::log_assertion(#__VA_ARGS__ " should have thrown " #type ", but has thrown something else"); }

#define MAYFLY_REQUIRE_NOTHROW(...) \
    try { __VA_ARGS__; } catch (...) { ::reaver::mayfly::log_assertion(#__VA_ARGS__ " shouldn't have thrown", true); }

#define MAYFLY_CHECK_NOTHROW(...) \
    try { __VA_ARGS__; } catch (...) { ::reaver::mayfly::log_assertion(#__VA_ARGS__ " shouldn't have thrown"); }
