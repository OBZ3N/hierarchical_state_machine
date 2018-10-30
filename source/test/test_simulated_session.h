
#pragma once

#include <string>

#include "test/test_simulated_event.h"
#include "test/test_interrupts.h"

namespace test
{
    class SimulatedSession
    {
    public:
        SimulatedSession( test::InterruptManager& interrupt_manager );

        enum class Status
        {
            Inactive,
            Joining,
            InSession,
            Leaving,
        };
        static const char* statusToString( Status status );

        void reset();

        void join();
        void update();

        test::SimulatedEvent    m_simul_wait_in_session;
        test::SimulatedEvent    m_simul_wait_leaving_session;
        test::SimulatedEvent    m_simul_wait_left_session;
        test::InterruptManager& m_interrupt_manager;

        Status m_status;
    };
}