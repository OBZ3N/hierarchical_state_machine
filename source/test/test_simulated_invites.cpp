
#include "test/test_simulated_invites.h"

namespace test
{
    SimulatedInvites::SimulatedInvites( InterruptManager& interrupt_manager )
        : m_interrupt_manager( interrupt_manager )
        , m_simul_wait_for_invite("invite_received", 0.0f, 300.0f, 0.0f, 0.05f, true)
        , m_simul_wait_for_accept("invite_accepted", 0.0f, 100.0f, 0.0f, 0.50f)
    {}
    
    void SimulatedInvites::start()
    {
        m_simul_wait_for_accept.stop();
        m_simul_wait_for_invite.stop();

        // start with no invite.
        m_simul_wait_for_invite.start();
    }

    void SimulatedInvites::update()
    {
        std::string event;

        if (m_simul_wait_for_invite.isStarted())
        {
            event = m_simul_wait_for_invite.update();

            // invite received.
            if (!event.empty())
            {
                // wait for accept.
                m_simul_wait_for_invite.stop();
                m_simul_wait_for_accept.start();
            }
        }

        if (m_simul_wait_for_accept.isStarted())
        {
            event = m_simul_wait_for_accept.update();

            // invite accepted. Needd to join invite.
            if (!event.empty())
            {
                // fire interrupt.
                m_interrupt_manager.raise( event );
                
                // restart waiting for a new invite.
                m_simul_wait_for_accept.stop();
                m_simul_wait_for_invite.start();
            }
        }
    }

    void SimulatedInvites::stop()
    {
        m_simul_wait_for_accept.stop();
        m_simul_wait_for_invite.stop();
    }
}