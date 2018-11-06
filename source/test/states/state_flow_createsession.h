
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
        class Flow_CreateSession : public test::StateBase
        {
        public:
            // factory.
            static const char* GetTypeName() { return "\\\\Flow\\CreateSession"; }
            static Flow_CreateSession* Instantiate(const hsm::schema::State& schema, test::Game& game ) { return new Flow_CreateSession( schema, game ); }

        public:
            Flow_CreateSession(const hsm::schema::State& schema, test::Game& game);
            ~Flow_CreateSession();

            virtual void enter() override;
            virtual std::string update() override;
            virtual void exit() override;

        private:

        };
    }
}