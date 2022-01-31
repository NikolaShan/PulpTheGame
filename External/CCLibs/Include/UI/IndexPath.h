//
//  IndexPath.h
//  Pulp
//
//  Created by Mikhail Shulepov on 26/07/15.
//
//

#pragma once

#include <stdlib.h>

using Index = ssize_t;
extern const Index IndexNotFound;

class IndexPath {
public:
    
    static const IndexPath NONE;
    constexpr static const Index NOT_AN_ITEM = -1;
    
    IndexPath() {}
    IndexPath(Index psection) : section(psection), item(NOT_AN_ITEM) {}
    IndexPath(Index psection, Index pitem) : section(psection), item(pitem) { }
    
    Index getSection() const { return section; }
    Index getItem() const { return item; }
    
    friend bool operator==(const IndexPath& lhs, const IndexPath& rhs);
    friend bool operator!=(const IndexPath& lhs, const IndexPath& rhs);
    friend bool operator< (const IndexPath& lhs, const IndexPath& rhs);
    friend bool operator> (const IndexPath& lhs, const IndexPath& rhs);
    friend bool operator<=(const IndexPath& lhs, const IndexPath& rhs);
    friend bool operator>=(const IndexPath& lhs, const IndexPath& rhs);
    
    bool isNone() const;
    bool isItem() const;
    
    int compare(const IndexPath &other) const {
        if (*this > other) {
            return 1;
        } else if (*this < other) {
            return -1;
        }
        return 0;
    }
    
private:
    Index section;
    Index item;
};