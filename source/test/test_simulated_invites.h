
#pragma once

#include <string>
#include "flow/debug.h"
#include "test/test_simulated_event.h"
#include "test/test_Interrupts.h"

namespace test
{
    class SimulatedInvites
    {
    public:
        SimulatedInvites( InterruptManager& interrupt_manager );

        void start();
        void update();
        void stop();

        test::SimulatedEvent m_simul_wait_for_invite;
        test::SimulatedEvent m_simul_wait_for_accept;
        test::InterruptManager& m_interrupt_manager;
    };
}