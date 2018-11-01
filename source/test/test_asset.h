
#include "hsm/asset.h"
#include "hsm/debug.h"

namespace test
{
    class AssetTest : public hsm::Asset
    {
    public:
        AssetTest( const hsm::schema::Asset& asset_schema );

        virtual bool load() override;
        virtual bool isLoaded() const override;
        virtual bool isLoading() const override;

        virtual bool unload() override;
        virtual bool isUnloaded() const override;
        virtual bool isUnloading() const override;

    private:
        debug::Timer m_load_timer;
        debug::Timer m_unload_timer;
        unsigned int m_load_completed_time;
        unsigned int m_unload_completed_time;

    };

}