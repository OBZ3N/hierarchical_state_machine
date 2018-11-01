
#include "test/test_factory.h"
#include "test/test_asset.h"
#include "test/test_game.h"

namespace test
{
    StateMachineFactory::StateMachineFactory(const hsm::schema::StateMachine& schema, test::Game& game )
        : m_schema(schema)
        , m_game(game)
    {
        generateStateFactoryTable(m_state_factory_table, game);
    }

    hsm::State* StateMachineFactory::createState( const hsm::schema::State& state_schema )
    {
        auto it = m_state_factory_table.find(state_schema.m_fullname);

        if (it == m_state_factory_table.end())
        {
            debug::logError("[TEST] Couldn't not instantiate state '%s'.", state_schema.m_fullname.c_str());
            return nullptr;
        }
        else
        {
            StateFactoryFunc func = it->second;

            hsm::State* state = func(state_schema, m_game);

            return state;
        }
    }

    std::list<hsm::Asset*> StateMachineFactory::createAssets( const std::list<hsm::schema::Asset>& asset_schemas )
    {
        std::list<hsm::Asset*> assets;

        for ( auto& asset_schema : asset_schemas )
        {
            hsm::Asset* asset = createAsset( asset_schema );

            assets.push_back( asset );
        }
        return assets;
    }

    std::list<hsm::Transition*> StateMachineFactory::createTransitions( const std::list<hsm::schema::Transition>& transition_schemas )
    {
        std::list<hsm::Transition*> transitions;

        for ( auto transition_schema : transition_schemas )
        {
            hsm::Transition* transition = createTransition( transition_schema );

            transitions.push_back( transition );
        }
        return transitions;
    }

    hsm::Asset* StateMachineFactory::createAsset( const hsm::schema::Asset& asset_schema )
    {
        hsm::Asset* asset = new AssetTest( asset_schema );

        return asset;
    }

    hsm::Transition* StateMachineFactory::createTransition( const hsm::schema::Transition& transition_schema )
    {
        hsm::Transition* transition = new hsm::Transition( transition_schema );

        return transition;
    }
}