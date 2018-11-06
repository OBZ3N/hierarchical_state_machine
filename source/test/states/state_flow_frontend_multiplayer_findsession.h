
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#pragma once

#include "hsm/schema.h"
#include "hsm/asset.h"
#include "hsm/state.h"
#include "hsm/debug.h"
#include "test/test_state_base.h"

namespace test
{
    namespace states
    {
        class Flow_FrontEnd_Multiplayer_FindSession : public test::StateBase
        {
        public:
            // factory.
            static const char* GetTypeName() { return "\\\\Flow\\FrontEnd\\Multiplayer\\FindSession"; }
            static Flow_FrontEnd_Multiplayer_FindSession* Instantiate(const hsm::schema::State& schema, test::Game& game ) { return new Flow_FrontEnd_Multiplayer_FindSession( schema, game ); }

        public:
            Flow_FrontEnd_Multiplayer_FindSession(const hsm::schema::State& schema, test::Game& game);
            ~Flow_FrontEnd_Multiplayer_FindSession();

            virtual void enter() override;
            virtual std::string update() override;
            virtual void exit() override;

        private:

        };
    }
}