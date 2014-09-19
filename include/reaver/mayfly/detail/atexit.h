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

#include <cstdlib>
#include <vector>
#include <atomic>

namespace reaver
{
    namespace mayfly { inline namespace _v1
    {
        namespace _detail
        {
            class _atexit_registry;
            _atexit_registry & _default_atexit_registry();

            class _atexit_registry
            {
            public:
                void atexit(void (*f)())
                {
                    _functions.push_back(f);
                }

                friend _atexit_registry & _default_atexit_registry();

            private:
                std::vector<void (*)()> _functions;
            };

            inline _atexit_registry & _default_atexit_registry()
            {
                static _atexit_registry registry;
                static std::atomic<bool> executed{ false };

                if (!executed)
                {
                    std::atexit([]
                    {
                        auto & registry = _default_atexit_registry();

                        for (auto it = registry._functions.rbegin(); it != registry._functions.rend(); ++it)
                        {
                            (*it)();
                        }
                    });

                    executed = true;
                }

                return registry;
            }

            static auto & _ = _default_atexit_registry();
        }
    }}
}
