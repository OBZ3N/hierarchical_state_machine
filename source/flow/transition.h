#pragma once

#include "flow/schema.h"

namespace flow
{
    class Transition
    {
    public:
        Transition( const schema::Transition& schema )
            : m_schema( schema )
        {}
        
        virtual ~Transition()
        {}

        const schema::Transition& GetSchema() const { return m_schema; }
        const std::string& getEvent() const { return m_schema.m_event_name; }
        const std::string& getNextState() const { return m_schema.m_next_state; }

    protected:
        const schema::Transition m_schema;
    };
}