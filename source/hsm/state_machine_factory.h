#pragma once

#include <functional>
#include <unordered_map>
#include "hsm/schema.h"
#include "hsm/resource.h"
#include "hsm/state.h"

namespace hsm
{
    // basic interface to factories, for states, resources, and transitions.
    class StateMachine;
    class StateMachineFactory
    {
    public:
        StateMachineFactory() {}
        virtual ~StateMachineFactory() {}

        virtual State* createState(StateMachine* state_machine, const schema::State& state_schema) = 0;
        virtual Resource* createResource(const schema::Resource& resource_schema) = 0;
    };
}