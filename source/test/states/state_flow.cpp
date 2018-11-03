
#include "test/states/state_flow.h"

namespace test
{
    namespace states
    {
        Flow::Flow( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
            , m_simul_boot_sequence("boot_sequence_completed", 0.0f, 2.0f)
        {}
        
        Flow::~Flow()
        {}

        void Flow::enter()
        {
            m_game.start();
            m_simul_boot_sequence.start();
        }

        std::string Flow::update()
        {
            m_game.update();

            std::string event = m_simul_boot_sequence.update();
            
            if(!event.empty())
            {
                return event;
            }

            return std::string();
        }

        void Flow::exit()
        {
            m_game.stop();

            m_simul_boot_sequence.stop();
        }
    }
}