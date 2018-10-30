
// Wednesday 17 October 2018, 14:12:33 : generated from 'Source\test\xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_pressstartscreen.h"

namespace test
{
    namespace states
    {
        Flow_PressStartScreen::Flow_PressStartScreen( const flow::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
            , m_simul_pressed_start("pressed_start", 0.0f, 10.0f)
            , m_simul_inactivity_timeout("inactivity_timeout", 5.0f, 5.0f)
        {}
        
        Flow_PressStartScreen::~Flow_PressStartScreen()
        {}

        void Flow_PressStartScreen::enter()
        {
             m_simul_pressed_start.start();
             m_simul_inactivity_timeout.start();
        }

        std::string Flow_PressStartScreen::update()
        {
            std::string event = m_simul_pressed_start.update();

            if (!event.empty())
            {
                return event;
            }

            event = m_simul_inactivity_timeout.update();

            if ( !event.empty() )
            {
                return event;
            }

            return std::string();
        }

        void Flow_PressStartScreen::exit()
        {

        }
    }
}