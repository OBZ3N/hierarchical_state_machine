
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
        class Flow_Race_TrackEnvironment_NextRaceVote : public test::StateBase
        {
        public:
            // factory.
            static const char* GetTypeName() { return "\\\\Flow\\Race\\TrackEnvironment\\NextRaceVote"; }
            static Flow_Race_TrackEnvironment_NextRaceVote* Instantiate(const hsm::schema::State& schema, test::Game& game ) { return new Flow_Race_TrackEnvironment_NextRaceVote( schema, game ); }

        public:
            Flow_Race_TrackEnvironment_NextRaceVote(const hsm::schema::State& schema, test::Game& game);
            ~Flow_Race_TrackEnvironment_NextRaceVote();

            virtual void enter() override;
            virtual std::string update() override;
            virtual void exit() override;

        private:

        };
    }
}