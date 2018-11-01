#pragma once

#include <string>
#include "hsm/schema.h"

namespace hsm
{
    // blocking process that will load assets before entering a state,
    // and unload assets after exiting a state. 
    // This is useful for loading resources prior to entering a specific state.
    // the structure is a stack, what is loaded last, will be unloaded first at exit.
    // example : loading dll modules at the boot sequence, 
    // memory allocation setup, initializing the network stack, singleton creations, 
    // loading level data, ui data, ect...
    class Asset
    {
    public:
        Asset( const schema::Asset& schema )
            : m_schema( schema )
        {}

        virtual ~Asset()
        {}

        const schema::Asset& getSchema() const { return m_schema; }


        virtual bool load() = 0;
        virtual bool isLoaded() const = 0;
        virtual bool isLoading() const = 0;

        virtual bool unload() = 0;
        virtual bool isUnloaded() const = 0;
        virtual bool isUnloading() const = 0;

    private:
        schema::Asset m_schema;
    };
}
