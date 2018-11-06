#pragma once

#include "hsm/schema.h"

namespace hsm
{
    // single state of the state machine.
    // update() will return an event that will interrupt the execution of the update stack, 
    // and fire a transition.
    class State
    {
    public:
        State( const schema::State& schema )
            : m_schema( schema )
        {}

        const schema::State& getSchema() const { return m_schema; }
        const std::string& getFullName() const { return m_schema.m_fullname; }

        virtual ~State()
        {}

        virtual void enter() = 0;
        virtual std::string update() = 0;
        virtual void exit() = 0;

    protected:
        const schema::State m_schema;
    };
}