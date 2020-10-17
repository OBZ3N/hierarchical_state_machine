#pragma once

#include <string>
#include "hsm/schema.h"

namespace hsm
{
    // blocking process that will load resources before entering a state,
    // and unload resources after exiting a state. 
    // This is useful for loading resources prior to entering a specific state.
    // the structure is a stack, what is loaded last, will be unloaded first at exit.
    // example : loading dll modules at the boot sequence, 
    // memory allocation setup, initializing the network stack, singleton creations, 
    // loading level data, ui data, ect...
    class Resource
    {
    public:
         Resource( const schema::Resource& schema )
            : m_schema( schema )
        {}

        virtual ~Resource()
        {}

        virtual void load() = 0;
        virtual void update() = 0;
        virtual void unload() = 0;

        const schema::Resource& getSchema() const { return m_schema; }

    private:
        schema::Resource m_schema;
    };
}
