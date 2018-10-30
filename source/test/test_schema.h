
#pragma once

#include <string>
#include <list>
#include <unordered_map>

namespace test
{
    namespace schema
    {
        struct Interrupt
        {
            std::string m_event_name;

            int m_level;
        };

        struct InterruptManager
        {
            std::list<Interrupt> m_interrupts;
        };

        struct Game
        {
            InterruptManager m_interrupt_manager;
        };
    }
}
