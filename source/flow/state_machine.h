#pragma once

#include <list>
#include <utility>
#include <unordered_map>
#include <string>

#include "flow/asset.h"
#include "flow/bitfield.h"
#include "flow/debug.h"
#include "flow/asset.h"
#include "flow/state.h"
#include "flow/transition.h"
#include "flow/state_machine_factory.h"

class TiXmlNode;

namespace flow
{
    class StateMachine
    {
    public:
        StateMachine( );
        ~StateMachine();

        bool load( const std::string& xml_filename );
        bool isLoaded() const;
                
        void start();
        void restart();
        void stop();

        bool isStopped() const;
        bool isStarted() const;

        void update();
        void shutdown();

        void setFactory(StateMachineFactory* factory);
        const schema::StateMachine& getSchema() const { return m_schema; }

    private:
        // components update.
        void updateStatesStack();
        void updateAssetsToLoad();
        void updateAssetsToUnload();
        void updateTransition();

        // utils.
        void evaluateTransitionOperation( std::string& state, bool& push_state, bool& pop_state ) const;
        void evaluateTransitionOperationBitfield( std::string& state, bool& push_state, bool& pop_state ) const;
        std::string evaluateCommonState(const std::string& state_a, const std::string& state_b) const;
        
        // debugging (extra string formating context).
        void logDebug( debug::LogLevel level, const char* format, ... ) const;

        // members.
        StateMachineFactory*    m_factory;          // create components (asset, state, transition...).
        schema::StateMachine    m_schema;           // state machine definition (loaded from xml).
        
        Transition*             m_transition;       // current transition.
        std::list<Asset*>       m_assetsToLoad;     // assets to load before entering a state.
        std::list<Asset*>       m_assetsToUnload;   // assets to unload before exiting a state.
        std::list<State*>       m_statesToUpdate;   // state stack to update.

        std::string             m_statusString;     // status of the state machine (debugging).

        // lookup table utils.
        std::vector<schema::State*> calculateSubStates( const schema::State* parent );
        void calculateLookupKey( const Bitfield& bitfield, const schema::State* state );
        bool calculateLookupTable();

        // accelerate the state search.
        std::unordered_map<Bitfield, std::string> m_state_lookup;
        std::unordered_map<std::string, Bitfield> m_state_reverse_lookup;
    };

}