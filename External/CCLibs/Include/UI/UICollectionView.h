//
//  UICollectionView.h
//  DaVinciQuiz
//
//  Created by mike on 05.05.14.
//
//

#ifndef __DaVinciQuiz__UICollectionView__
#define __DaVinciQuiz__UICollectionView__

#include <memory>
#include "extensions/GUI/CCScrollView/CCScrollView.h"
#include "Margin.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "UICollectionViewItem.h"
#include "IndexPath.h"
#include "renderer/CCGroupCommand.h"

NS_CC_BEGIN

class CollectionView : public extension::ScrollView {
public:
    using ItemNumber = size_t;
    class Delegate;
    class DataSource;
    static const Size DEFAULT_ITEM_SIZE;
    
    static CollectionView *create() {
        auto ret = new CollectionView();
        ret->init();
        ret->autorelease();
        return ret;
    }
    
    virtual ~CollectionView();
    
    virtual bool init() override;
    
    void setDelegate(Delegate *delegate);
    void setDataSource(DataSource *dataSource);
    void reloadData();
    
    void setReuseTemplateItem(CollectionViewItem *item, const std::string &reuseID);
    CollectionViewItem *getReusableItem(const std::string &reuseID);
    CollectionViewItem *getItemIfVisible(ItemNumber number);
    
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update(float dt) override;
    virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;
    
    virtual void setContentSize(const Size &size) override;
    void setDefaultItemSize(const Size &size);
    void setInterItemSpacing(const Size &size);
    void setContentMargin(const Margin &margin);
    
    void selectItem(ItemNumber item);
    void deselectItem(ItemNumber item);

    virtual bool onTouchBegan(Touch *touch, Event *unusedEvent) override;
    virtual void onTouchEnded(Touch *touch, Event *unusedEvent) override;
    
    cocos2d::Rect getItemPosition(ItemNumber item) const;
    
    void setAlignLeft(bool alignLeft);
    
protected:
    CollectionView();
    
private:
    class Implementation;
    typedef std::auto_ptr<Implementation> ImplPtr;
    ImplPtr pimpl;
    float prevScrollPosition;
    Size prevSize;
    cocos2d::GroupCommand groupCommand;
    
public:
    class Delegate {
    public:
        virtual ~Delegate() {}
        virtual void didSelectItem(CollectionView *view, ItemNumber item) = 0;
        virtual void didDeselectItem(CollectionView *iew, ItemNumber item) {}
    };
    
    class DataSource {
    public:
        virtual ~DataSource() {}
        virtual float widthForItem(CollectionView *view, ItemNumber item) { return -1.0f; }
        virtual Size sizeForItem(CollectionView *view, ItemNumber item) { return {widthForItem(view, item), -1.0f}; }
        virtual CollectionViewItem *viewForItem(CollectionView *view, ItemNumber item) = 0;
        virtual size_t numberOfItems(CollectionView *view) = 0;
    };
};

class CollectionViewLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~CollectionViewLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CollectionViewLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CollectionView);
};

NS_CC_END

#endif /* defined(__DaVinciQuiz__UICollectionView__) */
