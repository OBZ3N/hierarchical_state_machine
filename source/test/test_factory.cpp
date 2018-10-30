
#include "test/test_factory.h"
#include "test/test_asset.h"
#include "test/test_game.h"

namespace test
{
    StateMachineFactory::StateMachineFactory(const flow::schema::StateMachine& schema, test::Game& game )
        : m_schema(schema)
        , m_game(game)
    {
        generateStateFactoryTable(m_state_factory_table, game);
    }

    flow::State* StateMachineFactory::createState( const flow::schema::State& state_schema )
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

            flow::State* state = func(state_schema, m_game);

            return state;
        }
    }

    std::list<flow::Asset*> StateMachineFactory::createAssets( const std::list<flow::schema::Asset>& asset_schemas )
    {
        std::list<flow::Asset*> assets;

        for ( auto& asset_schema : asset_schemas )
        {
            flow::Asset* asset = createAsset( asset_schema );

            assets.push_back( asset );
        }
        return assets;
    }

    std::list<flow::Transition*> StateMachineFactory::createTransitions( const std::list<flow::schema::Transition>& transition_schemas )
    {
        std::list<flow::Transition*> transitions;

        for ( auto transition_schema : transition_schemas )
        {
            flow::Transition* transition = createTransition( transition_schema );

            transitions.push_back( transition );
        }
        return transitions;
    }

    flow::Asset* StateMachineFactory::createAsset( const flow::schema::Asset& asset_schema )
    {
        flow::Asset* asset = new AssetTest( asset_schema );

        return asset;
    }

    flow::Transition* StateMachineFactory::createTransition( const flow::schema::Transition& transition_schema )
    {
        flow::Transition* transition = new flow::Transition( transition_schema );

        return transition;
    }
}