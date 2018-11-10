#include <cstdarg>
#include <string>
#include <vector>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <windows.h>
#include <wincon.h>
#include "hsm/debug.h"
#include "hsm/random.h"

namespace utils
{
    void randomSeed( unsigned int seed )
    {
        srand( seed );
    }

    extern float randomFloat()
    {
        float rnd = rand() / (float)RAND_MAX;
        return rnd;
    }

    float randomFloat( float min, float max )
    {
        return min + randomFloat() * ( max - min );
    }
}