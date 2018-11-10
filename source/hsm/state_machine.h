#pragma once

#include <list>
#include <utility>
#include <unordered_map>
#include <string>

#include "hsm/asset.h"
#include "hsm/bitfield.h"
#include "hsm/debug.h"
#include "hsm/asset.h"
#include "hsm/state.h"
#include "hsm/transition.h"
#include "hsm/state_machine_factory.h"

class TiXmlNode;

namespace hsm
{
    class StateMachine
    {
    public:
        StateMachine( const schema::StateMachine& schema, StateMachineFactory* factory );
        ~StateMachine();

        void start( std::string& initial_state = std::string() );
        void restart( std::string& initial_state = std::string() );
        void stop();

        bool isStopped() const;
        bool isStarted() const;

        void update();
        void shutdown();

        StateMachineFactory* getFactory() { return m_factory; }
        const schema::StateMachine& getSchema() const { return m_schema; }

    private:
        // components update.
        void updateStatesStack();
        void updateAssetsToLoad();
        void updateAssetsToUnload();
        void updateTransition();

        // utils.
        void evaluateTransitionOperation( Bitfield& state_bitname, bool& push_state, bool& pop_state ) const;
        void evaluateTransitionOperation( std::string& state_fullname, bool& push_state, bool& pop_state ) const;

        // debugging (extra string formating context).
        void logDebug( debug::LogLevel level, const char* format, ... ) const;

        // members.
        StateMachineFactory*    m_factory;          // create components (asset, state, transition...).
        schema::StateMachine    m_schema;           // state machine definition (loaded from xml).
        
        Transition*             m_transition;       // current transition.
        Transition*             m_restart;          // transition for a restart.
        std::list<Asset*>       m_assetsToLoad;     // assets to load before entering a state.
        std::list<Asset*>       m_assetsToUnload;   // assets to unload before exiting a state.
        std::list<State*>       m_statesToUpdate;   // state stack to update.

        std::string             m_statusString;     // status of the state machine (debugging).

        // lookup table utils.
        // the state bitname is a bitfield representation, of the location of the state within the hierarchy. 
        // This is unique to each state, and can be use to accelerate transition evaluations (what state to push, pop, ect... to reach the desired state).
        std::vector<schema::State*> calculateSubStates( const schema::State* parent );
        void calculateLookupKey( const Bitfield& bitfield, const schema::State* state );
        bool calculateLookupTable();

        // lookup tables between state bitnames, and state fullnames.
        std::unordered_map<Bitfield, std::string> m_state_fullname_lookup;
        std::unordered_map<std::string, Bitfield> m_state_bitname_lookup;

#if defined(_DEBUG)
        // sanity checks. 
        // Use regular strings to evaluate if we need to push another state, or pop the current state off the stack to reach the desired state.
        void evaluateTransitionOperationWithFullNames(std::string& state_fullname, bool& push_state, bool& pop_state) const;
        std::string evaluateCommonState(const std::string& state_a, const std::string& state_b) const;
#endif
    };
}