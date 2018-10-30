#pragma once

#include <unordered_map>
#include "flow/state_machine_factory.h"

namespace test
{
    class Game;

    // factory functions.
    typedef std::function<flow::State*(const flow::schema::State&, test::Game&)> StateFactoryFunc;
    typedef std::unordered_map<std::string, StateFactoryFunc> StateFactoryTable;
    extern void generateStateFactoryTable(std::unordered_map<std::string, StateFactoryFunc>& state_factory_table, test::Game& game);

    class StateMachineFactory : public flow::StateMachineFactory
    {
    public:
        StateMachineFactory(const flow::schema::StateMachine& schema, test::Game& game);

        virtual flow::State*                  createState( const flow::schema::State& state_schema ) override;
        virtual std::list<flow::Asset*>       createAssets( const std::list<flow::schema::Asset>& asset_schemas ) override;
        virtual std::list<flow::Transition*>  createTransitions( const std::list<flow::schema::Transition>& transition_schemas ) override;

        virtual flow::Asset*                  createAsset( const flow::schema::Asset& asset_schema ) override;
        virtual flow::Transition*             createTransition( const flow::schema::Transition& transition_schema ) override;

    private:
        StateFactoryTable m_state_factory_table;

        test::Game& m_game;

        const flow::schema::StateMachine& m_schema;
    };
}