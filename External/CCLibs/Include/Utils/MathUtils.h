//
//  MathUtils.h
//  ColorTilt
//
//  Created by Mikhail Shulepov on 23/05/16.
//
//

#pragma once

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}