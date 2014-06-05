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

#include "testcase.h"
#include "reporter.h"

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        class suite
        {
        public:
            suite(std::string name, std::vector<testcase> testcases) : _name{ std::move(name) }, _testcases{ std::move(testcases) }
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
    }}
}
