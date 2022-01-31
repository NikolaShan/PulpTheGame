//
//  OnceToken.h
//  BlackHedgehog
//
//  Created by Mikhail Shulepov on 13.12.15.
//
//

#pragma once

#include <memory>

class OnceToken {
public:
    static std::shared_ptr<OnceToken> create() {
        return std::make_shared<OnceToken>();
    }
    
    OnceToken() {
        
    }    
    
    bool isUsed() const {
        return used;
    }
    
    void use() {
        used = true;
    }
    
private:
    bool used = false;
};