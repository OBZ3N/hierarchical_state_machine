
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_createsession.h"

namespace test
{
    namespace states
    {
        Flow_CreateSession::Flow_CreateSession( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_CreateSession::~Flow_CreateSession()
        {}

        void Flow_CreateSession::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_CreateSession");
        }

        std::string Flow_CreateSession::update()
        {
            return std::string();
        }

        void Flow_CreateSession::exit()
        {

        }
    }
}