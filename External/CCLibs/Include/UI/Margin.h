//
//  Margin.h
//  DaVinciQuiz
//
//  Created by mike on 11.05.14.
//
//

#ifndef __DaVinciQuiz__Margin__
#define __DaVinciQuiz__Margin__

class Margin {
public:
    Margin() : top(0.0f), left(0.0f), right(0.0f), bottom(0.0f) {}
    Margin(float pTop, float pLeft, float pRight, float pBottom)
    : top(pTop), left(pLeft), right(pRight), bottom(pBottom) {}
    
    float top, left, right, bottom;
};

#endif /* defined(__DaVinciQuiz__Margin__) */
