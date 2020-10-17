#pragma once

#include "hsm/schema.h"

namespace hsm
{
    // single state of the state machine.
    // update() will return an event that will interrupt the execution of the update stack, 
    // and fire a transition.
    class StateMachine;
    class State
    {
    public:
        State(StateMachine* parent, const schema::State& schema)
            : m_schema(schema)
            , m_parent(parent)
        {}

        virtual ~State()
        {}

        const schema::State& getSchema() const { return m_schema; }
        const std::string& getFullName() const { return m_schema.m_fullname; }

        StateMachine* getParent() { return m_parent; }
        const StateMachine* getParent() const { return m_parent; }

        virtual void enter() = 0;
        virtual void update() = 0;
        virtual void exit() = 0;

    protected:
        const schema::State m_schema;
        StateMachine* const m_parent;
    };
}