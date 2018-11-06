
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#pragma once

#include "hsm/schema.h"
#include "hsm/asset.h"
#include "hsm/state.h"
#include "hsm/debug.h"
#include "test/test_state_base.h"
#include "test/test_simulated_event.h"
#include "test/test_game.h"

namespace test
{
    namespace states
    {
        class Flow_AttractScreen : public test::StateBase
        {
        public:
            // factory.
            static const char* GetTypeName() { return "\\\\Flow\\AttractScreen"; }
            static Flow_AttractScreen* Instantiate(const hsm::schema::State& schema, test::Game& game ) { return new Flow_AttractScreen( schema, game ); }

        public:
            Flow_AttractScreen(const hsm::schema::State& schema, test::Game& game);
            ~Flow_AttractScreen();

            virtual void enter() override;
            virtual std::string update() override;
            virtual void exit() override;

        private:
            test::SimulatedEvent m_simul_attract_cancelled;
            test::SimulatedEvent m_simul_attract_timeout;
        };
    }
}