
#include "test/test_interrupts.h"
#include "hsm/debug.h"

namespace test
{
    Interrupt::Interrupt()
    {}

    Interrupt::Interrupt( const schema::Interrupt& schema )
        : m_schema( schema )
        , m_raised(false)
    {
    }
    
    InterruptManager::InterruptManager( const schema::InterruptManager& schema )
        : m_schema( schema )
    {
        for ( auto interrupt_schema : m_schema.m_interrupts )
        {
            m_interrupts[ interrupt_schema.m_event_name ] = Interrupt( interrupt_schema );
        }
    }

    
    void InterruptManager::raise( const std::string& event_name )
    {
        auto& it = m_interrupts.find( event_name );

        if ( it != m_interrupts.end() )
        {
            Interrupt& interrupt = it->second;

            for ( auto& jt : m_interrupts )
            {
                if ( jt.second.getSchema().m_level <= interrupt.getSchema().m_level )
                {
                    debug::logTrace( "[TEST] consumed interrupt '%s' of a lower level.", jt.second.getSchema().m_event_name.c_str());

                    jt.second.setRaised( false );
                }
            }

            debug::logTrace( "[TEST] interrupt '%s' raised with level %d.", interrupt.getSchema().m_event_name.c_str(), interrupt.getSchema().m_level );

            interrupt.setRaised( true );
        }
    }

    void InterruptManager::consume( const std::string& event_name )
    {
        auto& it = m_interrupts.find( event_name );

        if ( it != m_interrupts.end() )
        {
            Interrupt& interrupt = it->second;

            interrupt.setRaised( false );

            debug::logTrace( "[TEST] interrupt '%s' consumed.", interrupt.getSchema().m_event_name.c_str() );
        }
    }

    bool InterruptManager::isRaised( const std::string& event_name ) const
    {
        auto it = m_interrupts.find( event_name );

        if ( it == m_interrupts.end() )
            return false;

        const Interrupt& interrupt = it->second;

        return interrupt.isRaised();
    }

    void InterruptManager::reset()
    {
        for (auto it : m_interrupts)
        {
            it.second.setRaised(false);
        }

    }
}