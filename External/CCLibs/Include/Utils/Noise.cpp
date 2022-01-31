//
//  Noise.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 22/02/16.
//
//

#include "Noise.h"
#include "Math/MathUtil.h"

namespace Noise {
    namespace Perlin {
        //https://mrl.nyu.edu/~perlin/doc/oscar.html
        /* coherent noise function over 1, 2 or 3 dimensions */
        /* (copyright Ken Perlin) */
        
        static constexpr size_t B = 0x100;
        static constexpr size_t BM = 0xff;
        static constexpr size_t N = 0x1000;
        static constexpr size_t NP = 12; /* 2^N */
        static constexpr size_t NM = 0xfff;
        
        static int p[B + B + 2];
        static float g3[B + B + 2][3];
        static float g2[B + B + 2][2];
        static float g1[B + B + 2];
        static int start = 1;
        
        static void init(void);
        
        inline float s_curve(float t) { return ( t * t * (3. - 2. * t) ); }
        //inline float lerp(float t, float a, float b) { return ( a + t * (b - a) ); }
        inline void setup(size_t i, float *vec, int &b0, int &b1, float &r0, float &r1, float &t) {
            t = vec[i] + N;
            b0 = ((int)t) & BM;
            b1 = (b0 + 1) & BM;
            r0 = t - (int)t;
            r1 = r0 - 1.0;
        }
        
        float noise1(float arg) {
            int bx0, bx1;
            float rx0, rx1, sx, t, u, v;
            
            if (start) {
                start = 0;
                init();
            }
            
            setup(0, &arg, bx0,bx1, rx0,rx1, t);
            
            sx = s_curve(rx0);
            
            u = rx0 * g1[ p[ bx0 ] ];
            v = rx1 * g1[ p[ bx1 ] ];
            
            return cocos2d::MathUtil::lerp(u, v, sx);
        }
        
        float noise2(float vec[2]) {
            int bx0, bx1, by0, by1, b00, b10, b01, b11;
            float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
            size_t i, j;
            
            if (start) {
                start = 0;
                init();
            }
            
            setup(0, vec, bx0,bx1, rx0,rx1, t);
            setup(1, vec, by0,by1, ry0,ry1, t);
            
            i = p[ bx0 ];
            j = p[ bx1 ];
            
            b00 = p[ i + by0 ];
            b10 = p[ j + by0 ];
            b01 = p[ i + by1 ];
            b11 = p[ j + by1 ];
            
            sx = s_curve(rx0);
            sy = s_curve(ry0);
            
#define at2(rx,ry) ( rx * q[0] + ry * q[1] )
            
            q = g2[ b00 ] ; u = at2(rx0,ry0);
            q = g2[ b10 ] ; v = at2(rx1,ry0);
            a = cocos2d::MathUtil::lerp(u, v, sx);
            
            q = g2[ b01 ] ; u = at2(rx0,ry1);
            q = g2[ b11 ] ; v = at2(rx1,ry1);
            b = cocos2d::MathUtil::lerp(u, v, sx);
            
            return cocos2d::MathUtil::lerp(sy, a, b);
        }
        
        float noise3(float vec[3]) {
            int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
            float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
            size_t i, j;
            
            if (start) {
                start = 0;
                init();
            }
            
            setup(0, vec, bx0,bx1, rx0,rx1, t);
            setup(1, vec, by0,by1, ry0,ry1, t);
            setup(2, vec, bz0,bz1, rz0,rz1, t);
            
            i = p[ bx0 ];
            j = p[ bx1 ];
            
            b00 = p[ i + by0 ];
            b10 = p[ j + by0 ];
            b01 = p[ i + by1 ];
            b11 = p[ j + by1 ];
            
            t  = s_curve(rx0);
            sy = s_curve(ry0);
            sz = s_curve(rz0);
            
#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )
            
            q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
            q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
            a = cocos2d::MathUtil::lerp(u, v, t);
            
            q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
            q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
            b = cocos2d::MathUtil::lerp(u, v, t);
            
            c = cocos2d::MathUtil::lerp(a, b, sy);
            
            q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
            q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
            a = cocos2d::MathUtil::lerp(u, v, t);
            
            q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
            q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
            b = cocos2d::MathUtil::lerp(u, v, t);
            
            d = cocos2d::MathUtil::lerp(a, b, sy);
            
            return cocos2d::MathUtil::lerp(c, d, sz);
        }
        
        static void normalize2(float v[2]) {
            float s;
            
            s = sqrt(v[0] * v[0] + v[1] * v[1]);
            v[0] = v[0] / s;
            v[1] = v[1] / s;
        }
        
        static void normalize3(float v[3]) {
            float s;
            
            s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            v[0] = v[0] / s;
            v[1] = v[1] / s;
            v[2] = v[2] / s;
        }
        
        static void init(void) {
            int i, j, k;
            
            for (i = 0 ; i < B ; i++) {
                p[i] = i;
                
                g1[i] = (float)((random() % (B + B)) - B) / B;
                
                for (j = 0 ; j < 2 ; j++)
                    g2[i][j] = (float)((random() % (B + B)) - B) / B;
                normalize2(g2[i]);
                
                for (j = 0 ; j < 3 ; j++)
                    g3[i][j] = (float)((random() % (B + B)) - B) / B;
                normalize3(g3[i]);
            }
            
            while (--i) {
                k = p[i];
                p[i] = p[j = random() % B];
                p[j] = k;
            }
            
            for (i = 0 ; i < B + 2 ; i++) {
                p[B + i] = p[i];
                g1[B + i] = g1[i];
                for (j = 0 ; j < 2 ; j++)
                    g2[B + i][j] = g2[i][j];
                for (j = 0 ; j < 3 ; j++)
                    g3[B + i][j] = g3[i][j];
            }
        }
    }

    float perlinNoise1D(float x) {
        return Perlin::noise1(x);
    }
    
    float perlinNoise2D(float x, float y) {
        float v[2] = {x, y};
        return Perlin::noise2(v);
    }
    
    float perlinNoise3D(float x, float y, float z) {
        float v[3] = {x, y, z};
        return Perlin::noise3(v);
    }
}
