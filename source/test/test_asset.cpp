
#include "test/test_asset.h"
#include <algorithm>

namespace test
{
    AssetTest::AssetTest( const hsm::schema::Asset& asset_schema )
        : Asset( asset_schema )
    {
        m_load_completed_time = debug::random<unsigned int>( 100, 2000 );
        m_unload_completed_time = debug::random<unsigned int>( 10, 200 );
    }

    bool AssetTest::load()
    {
        m_load_timer.restart();
        return true;
    }

    bool AssetTest::isLoading() const
    {
        return m_load_timer.isStarted();
    }

    bool AssetTest::unload()
    {
        m_unload_timer.restart();
        return true;
    }

    bool AssetTest::isLoaded() const
    {
        return m_load_timer.getElapsedTimeMSecs() >= m_load_completed_time;
    }

    bool AssetTest::isUnloading() const 
    {
        return m_unload_timer.isStarted();
    }

    bool AssetTest::isUnloaded() const
    {
        return m_unload_timer.getElapsedTimeMSecs() >= m_unload_completed_time;
    }
}