#pragma once

#include <functional>
#include <unordered_map>
#include "flow/schema.h"
#include "flow/asset.h"
#include "flow/transition.h"
#include "flow/state.h"

namespace flow
{
    // basic interface to factories, for states, assets, and transitions.
    class StateMachineFactory
    {
    public:
        StateMachineFactory() {}
        virtual ~StateMachineFactory() {}

        virtual State*                  createState(const schema::State& state_schema) = 0;
        virtual std::list<Asset*>       createAssets(const std::list<schema::Asset>& asset_schemas) = 0;
        virtual std::list<Transition*>  createTransitions(const std::list<schema::Transition>& transition_schemas) = 0;
        
        virtual Asset*                  createAsset(const schema::Asset& asset_schema) = 0;
        virtual Transition*             createTransition(const schema::Transition& transition_schema) = 0;
    };
}