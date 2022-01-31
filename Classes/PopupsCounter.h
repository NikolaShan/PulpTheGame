//
//  PopupsCounter.hpp
//  Pulp
//
//  Created by Mikhail Shulepov on 25.11.15.
//
//

#pragma once

class PopupsCounter {
public:
    static PopupsCounter *getInstance() {
        static auto instance = new PopupsCounter;
        return instance;
    }
    
    void incrementPlaysCount() {
        playsWithoutPopups++;
    }
    
    void resetPlaysWithoutPopups() {
        playsWithoutPopups = 0;
    }
    
    size_t getPlaysCountWithoutPopups() const {
        return playsWithoutPopups;
    }
    
private:
    size_t playsWithoutPopups = 0;
};