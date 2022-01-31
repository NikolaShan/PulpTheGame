//
//  DynamicValue.h
//  Pulp
//
//  Created by Mikhail Shulepov on 21/07/15.
//
//

#pragma once

#include <vector>
#include <memory>
#include "base/CCMacros.h"



class DynamicValue {
public:
    virtual ~DynamicValue() {}
    virtual void touch() {}
    virtual void update(float dt) {};
    virtual float getFloatValue() const = 0;
};

using DynamicValuePtr = std::shared_ptr<DynamicValue>;

///
/// Always return the same value
///
class ConstantValue: public DynamicValue {
public:
    ConstantValue(float pvalue): value(pvalue) { }
    
    virtual float getFloatValue() const override {
        return value;
    }
    
private:
    float value;
};


///
/// May return new value after touch
///
class QueryDependedValue: public DynamicValue {
public:
    QueryDependedValue(const std::vector<float> &pvalues)
    : values(pvalues)
    , currentIdx(0) {
        
    }
    
    virtual float getFloatValue() const override {
        return values[currentIdx];
    }
    
    virtual void touch() override {
        currentIdx = (currentIdx + 1) % values.size();
    }
    
private:
    mutable size_t currentIdx = 0;
    std::vector<float> values;
};


///
/// May return different values based on passed time
///
class TimeDependedValue: public DynamicValue {
public:
    TimeDependedValue(const std::vector<float> &pvalues, const std::vector<float> &pdelays)
    : values(pvalues)
    , elapsedTime(0.0f) {
        CCASSERT(pvalues.size() == pdelays.size(), "Count should be the same");
        float lastTime = 0.0f;
        for (float delay: pdelays) {
            lastTime += delay;
            times.push_back(lastTime);
        }
        currentIdx = 0;
        nextTime = times.front();
    }
    
    virtual void update(float dt) override {
        const float prevElapsedTime = elapsedTime;
        elapsedTime += dt;
        if (elapsedTime > nextTime) {
            const float prevValue = values[currentIdx];
            
            currentIdx++;
            if (currentIdx >= values.size()) {
                currentIdx = 0;
            }
            
            const float nextValue = values[currentIdx];
            const float elapsedTimeDelta = elapsedTime - prevElapsedTime;
            value = ((nextTime - prevElapsedTime) * prevValue + (elapsedTime - nextTime) * nextValue) / elapsedTimeDelta;
            
            nextTime = times[currentIdx];
            
            if (currentIdx == 0) {
                elapsedTime -= times.back();
            }
            
        } else {
            value = values[currentIdx];
        }
    };
    
    virtual float getFloatValue() const override {
        return value;
    }
    
private:
    float value;
    
    size_t currentIdx = 0;
    float nextTime = 0;
    float elapsedTime;
    std::vector<float> values;
    std::vector<float> times;
};