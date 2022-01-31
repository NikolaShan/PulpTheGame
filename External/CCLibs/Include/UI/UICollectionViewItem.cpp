//
//  UICollectionViewItem.cpp
//  DaVinciQuiz
//
//  Created by mike on 05.05.14.
//
//

#include "UICollectionViewItem.h"

NS_CC_BEGIN;

CollectionViewItem::CollectionViewItem() {
    isSelectedState = false;
    isHighlightedState = false;
}
    
CollectionViewItem *CollectionViewItem::clone() const {
    auto ret = createCloneInstance();
    ret->copySpecialProperties(this);
    return ret;
}

void CollectionViewItem::copySpecialProperties(CollectionViewItem const *other) {
    setReuseID(other->getReuseID());
}

NS_CC_END