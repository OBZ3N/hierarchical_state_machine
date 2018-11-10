
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"
#include "test/test_simulated_event.h"
#include "test/states/state_flow_attractscreen.h"

namespace test
{
    namespace states
    {
        Flow_AttractScreen::Flow_AttractScreen( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
            , m_simul_attract_cancelled( "attract_cancelled", 0.0f, 10.0f )
            , m_simul_attract_timeout( "attract_timeout", 15.0f, 15.0f )
        {}
        
        Flow_AttractScreen::~Flow_AttractScreen()
        {}

        void Flow_AttractScreen::enter()
        {
            m_simul_attract_cancelled.start();
            m_simul_attract_timeout.start();
        }

        std::string Flow_AttractScreen::update()
        {
            if ( m_game.m_interrupt_manager->isRaised( "invite_accepted" ) )
            {
                return "invite_accepted";
            }

            std::string event = m_simul_attract_cancelled.update();

            if ( !event.empty() )
            {
                return event;
            }

            event = m_simul_attract_timeout.update();

            if ( !event.empty() )
            {
                return event;
            }

            return "";
        }

        void Flow_AttractScreen::exit()
        {
            m_simul_attract_cancelled.stop();
            m_simul_attract_timeout.stop();
        }
    }
}