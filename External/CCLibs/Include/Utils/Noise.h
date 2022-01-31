//
//  Noise.hpp
//  Pulp
//
//  Created by Mikhail Shulepov on 22/02/16.
//
//

#pragma once

namespace Noise {
    
    
    float perlinNoise1D(float x);
    float perlinNoise2D(float x, float y);
    float perlinNoise3D(float x, float y, float z);
}