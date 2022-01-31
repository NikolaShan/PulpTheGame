//
//  IndexPath.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 26/07/15.
//
//


#include "UI/IndexPath.h"

const Index IndexNotFound = -1;

const IndexPath IndexPath::NONE = IndexPath(NOT_AN_ITEM, NOT_AN_ITEM);

bool IndexPath::isNone() const {
    return section == NOT_AN_ITEM && item == NOT_AN_ITEM;
}

bool IndexPath::isItem() const {
    return item != NOT_AN_ITEM;
}

bool operator==(const IndexPath& lhs, const IndexPath& rhs){
    return lhs.section == rhs.section && lhs.item == rhs.item;
}

bool operator!=(const IndexPath& lhs, const IndexPath& rhs){
    return !(lhs == rhs);
}

bool operator< (const IndexPath& lhs, const IndexPath& rhs){
    return lhs.section < rhs.section || lhs.item < rhs.item;
}

bool operator> (const IndexPath& lhs, const IndexPath& rhs){
    return  rhs < lhs;
}

bool operator<=(const IndexPath& lhs, const IndexPath& rhs){
    return !(lhs > rhs);
}

bool operator>=(const IndexPath& lhs, const IndexPath& rhs){
    return !(lhs < rhs);
}
