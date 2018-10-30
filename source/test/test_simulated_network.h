
#pragma once

#include <string>


#include "test/test_simulated_event.h"
#include "test/test_interrupts.h"

namespace test
{
    class SimulatedNetwork
    {
    public:
        SimulatedNetwork( test::InterruptManager& interrupt_manager );

        void start();
        void update();
        void stop();

        test::SimulatedEvent m_simul_wait_connected;
        test::SimulatedEvent m_simul_wait_disconnected;
        test::InterruptManager& m_interrupt_manager;
    };
}