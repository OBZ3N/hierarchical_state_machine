#pragma once

// basic debugging utils.
namespace utils
{
    // pseudo-random functions.
    extern void randomSeed( unsigned int seed );

    extern float randomFloat();

    extern float randomFloat( float min, float max );

    template<typename type> type random( int a, int b )
    {
        float rnd = rand() / (float)RAND_MAX;
        return (type)( a + rnd * ( b - a ) );
    }
}