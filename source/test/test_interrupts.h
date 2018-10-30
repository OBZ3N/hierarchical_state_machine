
#pragma once

#include <string>
#include <unordered_map>
#include "test/test_schema.h"

namespace test
{
    class Interrupt
    {
    public:
        Interrupt();

        Interrupt( const test::schema::Interrupt& schema );

        const test::schema::Interrupt& getSchema() const { return m_schema; }

        void setRaised( bool raised ) { m_raised = raised; }
        
        bool isRaised() const { return m_raised; }
    
    private:
        schema::Interrupt m_schema;

        bool m_raised;
    };

    class InterruptManager
    {
    public:
        InterruptManager( const schema::InterruptManager& schema );

        void reset();

        bool isRaised( const std::string& event_name ) const;

        void raise( const std::string& event_name );

        void consume( const std::string& event_name );
    
    private:
        schema::InterruptManager m_schema;

        std::unordered_map<std::string, Interrupt> m_interrupts;
    };
}