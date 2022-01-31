//
//  UICollectionViewItem.h
//  DaVinciQuiz
//
//  Created by mike on 05.05.14.
//
//

#ifndef __DaVinciQuiz__UICollectionViewItem__
#define __DaVinciQuiz__UICollectionViewItem__

#include "2d/CCNode.h"

NS_CC_BEGIN

class CollectionViewItem : public Node, public Clonable {
public:
    CollectionViewItem();
    virtual ~CollectionViewItem() {}
    void setHighlighted(bool highlighted) {
        if (isHighlightedState != highlighted) {
            isHighlightedState = highlighted;
            _updateSelectionState(true);
        }
    }
    
    void setSelected(bool selected, bool animated) {
        if (isSelectedState != selected) {
            isSelectedState = selected;
            _updateSelectionState(animated);
        }
    }
    
    bool isSelected() const { return isSelectedState; }
    bool isHighlighted() const { return isHighlightedState; }

    void setReuseID(const std::string &pReuseId) { reuseID = pReuseId; }
    const std::string &getReuseID() const { return reuseID; }
    
    virtual CollectionViewItem *clone() const override;
    virtual CollectionViewItem *createCloneInstance() const = 0;
    virtual void copySpecialProperties(CollectionViewItem const *model);
    
protected:
    virtual void updateSelectionState(bool selected, bool animated) {}
    
private:
    void _updateSelectionState(bool animated) {
        const bool selected  = (isHighlightedState != isSelectedState);
        updateSelectionState(selected, animated);
    }
    
private:
    std::string reuseID;
    bool isSelectedState;
    bool isHighlightedState;
};

NS_CC_END


#endif /* defined(__DaVinciQuiz__UICollectionViewItem__) */
