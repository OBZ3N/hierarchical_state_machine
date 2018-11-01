
#include "hsm/debug.h"
#include "test/test_simulated_session.h"

namespace test
{
    const char* SimulatedSession::statusToString( Status status )
    {
        switch ( status )
        {
        default:                return "????";
        case Status::Inactive:  return "Inactive";
        case Status::Joining:   return "Joining";
        case Status::InSession: return "InSession";
        case Status::Leaving:   return "Leaving";
        }
    }

    SimulatedSession::SimulatedSession( test::InterruptManager& interrupt_manager )
        : m_interrupt_manager( interrupt_manager )
        , m_status( Status::Inactive )
        , m_simul_wait_in_session( "session_in_session", 3.0f, 20.0f )
        , m_simul_wait_leaving_session( "session_leaving", 0.0f, 300.0f, 0.0f, 0.05f, true )
        , m_simul_wait_left_session( "session_leaving", 3.0f, 5.0f )
    {}

    void SimulatedSession::reset()
    {
        m_simul_wait_in_session.stop();
        m_simul_wait_leaving_session.stop();
        m_simul_wait_left_session.stop();
        
        m_status = Status::Inactive;
    }

    void SimulatedSession::join()
    {
        ASSERT_SANITY( m_status != Status::Inactive, "[TEST] [SESSION] Cannot join session in status '%s'.", statusToString( m_status ) );

        m_simul_wait_in_session.start();
        m_status = Status::Joining;
    }

    void SimulatedSession::update()
    {
        if ( m_simul_wait_in_session.isStarted() )
        {
            std::string event = m_simul_wait_in_session.update();

            if ( !event.empty() )
            {
                m_simul_wait_in_session.stop();
                m_simul_wait_leaving_session.start();
                m_status = Status::InSession;
                debug::logInfo( "[TEST] [SESSION] in session." );
            }
        }

        if ( m_simul_wait_leaving_session.isStarted() )
        {
            std::string event = m_simul_wait_leaving_session.update();

            if ( !event.empty() )
            {
                m_simul_wait_leaving_session.stop();
                m_simul_wait_left_session.start();
                m_status = Status::Leaving;
                debug::logInfo( "[TEST] [SESSION] leaving session." );
            }
        }

        if ( m_simul_wait_left_session.isStarted() )
        {
            std::string event = m_simul_wait_left_session.update();

            if ( !event.empty() )
            {
                m_simul_wait_left_session.stop();
                m_status = Status::Inactive;
                debug::logInfo( "[TEST] [SESSION] left session." );
            }
        }
    }
}