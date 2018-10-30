
#include "test/test_simulated_online_services.h"

namespace test
{
    SimulatedOnlineServices::SimulatedOnlineServices( test::InterruptManager& interrupt_manager )
        : m_interrupt_manager( interrupt_manager )
        , m_simul_wait_connected( "online_services_connected", 0.0f, 100.0f )
        , m_simul_wait_disconnected( "online_services_disconnected", 0.0f, 200.0f, 0.0f, 0.05f, true )
    {}
    
    void SimulatedOnlineServices::start()
    {
        m_simul_wait_connected.stop();
        m_simul_wait_connected.stop();

        // start connected.
        m_simul_wait_disconnected.start();
    }

    void SimulatedOnlineServices::update()
    {
        std::string event;

        if ( m_simul_wait_connected.isStarted() )
        {
            event = m_simul_wait_connected.update();

            // cable reconnected.
            if ( !event.empty() )
            {
                // wait for a new disconnection.
                m_simul_wait_connected.stop();
                m_simul_wait_disconnected.start();
            }
        }

        if ( m_simul_wait_disconnected.isStarted() )
        {
            event = m_simul_wait_disconnected.update();

            // cable disconnected.
            if ( !event.empty() )
            {
                // wait for a reconnection.
                m_interrupt_manager.raise( event );

                m_simul_wait_disconnected.stop();
                m_simul_wait_connected.start();
            }
        }
    }

    void SimulatedOnlineServices::stop()
    {
        m_simul_wait_disconnected.stop();
        m_simul_wait_connected.stop();
    }
}