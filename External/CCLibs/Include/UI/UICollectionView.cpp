//
//  UICollectionView.cpp
//  DaVinciQuiz
//
//  Created by mike on 05.05.14.
//
//

#include <algorithm>

#include "base/CCMap.h"
#include "base/CCVector.h"
#include "base/CCTouch.h"
#include "renderer/CCRenderer.h"

#include "Utils/Collections.h"

#include "UICollectionView.h"
#include "UICollectionViewItem.h"

NS_CC_BEGIN

const Size CollectionView::DEFAULT_ITEM_SIZE = Size(44.0f, 44.0f);
    
#pragma mark -
#pragma mark Implementation
    
class CollectionView::Implementation {
public:
    Implementation(CollectionView *pOwner) : owner(pOwner) {
        defaultItemSize = DEFAULT_ITEM_SIZE;
        allowedMultipleSelection = false;
        dataSource = nullptr;
        delegate = nullptr;
        alignLeft = true;
    }
    ~Implementation() {
        
    }
    void setDelegate(CollectionView::Delegate *pDelegate) { delegate = pDelegate; }
    void setDataSource(CollectionView::DataSource *pDataSource) { dataSource = pDataSource; }
    void setDefaultItemSize(const Size &size) { defaultItemSize = size; }
    void setInterItemSpacing(const Size &size) { minInterItemSpacing = size; }
    void setContentMargin(const Margin &margin) { contentMargin = margin; }
    
    void allowMultipleSelection(bool allow) { allowedMultipleSelection = allow; }
    void selectItem(ItemNumber item, bool animated);
    void deselectItem(ItemNumber item, bool animated);
    const std::vector<ItemNumber> &getSelectedItems() const { return selectedItems; }
    bool isItemSelected(ItemNumber item) const;
    
    void highlightItem(int touchID, ItemNumber item);
    void unhighlightItem(int touchID);
    
    void updateVisibleItems();
    void reloadData();

    void updatePositions();
   
    void setReuseWidget(CollectionViewItem *widget, const std::string &reuseID);
    CollectionViewItem *getReusableWidget(const std::string &reuseID);
    std::vector<ItemNumber> getItemsInRows(const std::vector<size_t> &rows) const;
    std::vector<size_t> getVisibleRows() const;
    ItemNumber getItemAtPosition(const Vec2 &pos, bool &contains);
    CollectionViewItem *getWidgetIfVisible(ItemNumber item);
    
    size_t getItemNumberForIndexPath(const IndexPath &path) const;
    IndexPath getIndexPathForItemNumber(size_t itemNumber) const;
    
    Rect getItemPosition(ItemNumber item) const;
    
private:
    CollectionViewItem *getWidget(ItemNumber index);
    void freeItems(const std::vector<ItemNumber> &toFree);
    
public:
    bool alignLeft;
    
private:
    CollectionView *owner;
    Delegate *delegate;
    DataSource *dataSource;
    
    bool allowedMultipleSelection;
    std::vector<ItemNumber> selectedItems;
    std::map<int, ItemNumber> highlightedItems;
    
    Margin contentMargin;
    Size minInterItemSpacing;
    Size defaultItemSize;
    
    std::vector<ItemNumber> rows;
    std::vector<float> rowsPositions;
    
    std::vector<Size> itemsSizes;
    std::vector<Vec2> itemPositions;
    
    Map<std::string, CollectionViewItem *> reuseTemplates;
    std::map<std::string, Vector<CollectionViewItem *>> freedWidgets;
    
    std::vector<ItemNumber> visibleItems;
    Vector<CollectionViewItem *> visibleWidgets;
    
    // idx of first item in section, e.g. [0, 3, 8, 20]
    // That means that first section contains 3 items, second - 5 items, third - 12 items
    std::vector<size_t> sections;
};


#pragma mark Updating and Reloading
    
void CollectionView::Implementation::reloadData() {
    freeItems(visibleItems);
    visibleItems.clear();
    itemsSizes.clear();
    
    if (dataSource) {
        const size_t itemsCount = dataSource->numberOfItems(owner);
        for (ItemNumber i = 0; i < itemsCount; ++i) {
            Size size = dataSource->sizeForItem(owner, i);
            if (size.width < 0) {
                size.width = defaultItemSize.width;
            }
            
            if (size.height < 0) {
                size.height = defaultItemSize.height;
            }
            
            itemsSizes.push_back(size);
        }
    }

    updatePositions();
    if (owner && owner->getContainer()) {
        owner->setContentOffset({0.0f, owner->getViewSize().height - owner->getContainer()->getContentSize().height});
    }
}

/// Cache positions for all items so them can be queried fastly
void CollectionView::Implementation::updatePositions() {
    // Clean cached data
    itemPositions.clear();
    rows.clear();
    
    // Just remove some items from screen if necessary
    if (itemsSizes.empty()) {
        updateVisibleItems();
        return;
    }
    
    const float availableWidth = owner->getViewSize().width;
    if (availableWidth < 1) {
        return;
    }
    
    // Estimate positions
    float currentWidth = 0;
    float currentY = contentMargin.top;
    float currentRowHeight = 0;
    for (ItemNumber item = 0, count = itemsSizes.size(); item < count; ++item) {
        const Size itemSize = itemsSizes[item];
        const float newWidth = currentWidth + minInterItemSpacing.width + itemSize.width;
        const bool isNewRow = newWidth > availableWidth || item == 0;
        if (isNewRow) {
            rows.push_back(item);
            currentWidth = contentMargin.left + itemSize.width;
            if (item != 0) {
                currentY += minInterItemSpacing.height + currentRowHeight;
            }
            currentRowHeight = itemSize.height;
        } else {
            currentRowHeight = std::max(currentRowHeight, itemSize.height);
            currentWidth = newWidth;
        }
        
        const Vec2 position(currentWidth - itemSize.width, currentY);
        itemPositions.push_back(position);
    }
    
    //update horizontal spacing
    float prevExtraSpace = 0;
    for (size_t row = 0; row < rows.size(); ++row) {
        auto itemsInRow = getItemsInRows({row});
        if (itemsInRow.size() == 1) {
            if (alignLeft) {
                itemPositions[itemsInRow.front()].x = contentMargin.left;//left side
            } else {
                itemPositions[itemsInRow.front()].x = (availableWidth - itemsSizes[itemsInRow.front()].width) / 2;//center
            }
            
        } else {
            const ItemNumber firstItem = itemsInRow.front();
            const ItemNumber lastItem = itemsInRow.back();
            const float xBegin = itemPositions[firstItem].x;
            const float xEnd = itemPositions[lastItem].x + itemsSizes[itemsInRow.back()].width;
            const float freeSpace = availableWidth - (xEnd - xBegin) - contentMargin.right - contentMargin.left;
            float extraItemsSpace =  freeSpace / (itemsInRow.size() - 1);
            if (alignLeft && row == rows.size() - 1) {
                extraItemsSpace = prevExtraSpace;
            }
            for (ItemNumber  i = 1; i < itemsInRow.size(); ++i) {
                ItemNumber item = firstItem + i;
                itemPositions[item].x += extraItemsSpace * i;
            }
            prevExtraSpace = extraItemsSpace;
        }
    }
    
    //update total content size
    const float requireHeight = itemPositions.back().y + currentRowHeight + contentMargin.bottom;
    const float resultHeight = std::max(requireHeight, owner->getViewSize().height);
    owner->getContainer()->setContentSize({owner->getViewSize().width, resultHeight});
    
    //reverse fill order
    for (auto &position : itemPositions) {
        position.y = resultHeight - position.y;
    }
    
    rowsPositions.clear();
    for (ItemNumber row : rows) {
        rowsPositions.push_back(itemPositions[row].y);
    }
    
    updateVisibleItems();
}

/// Remove items that become invisible offscreen
/// Add new visible items to screen
void CollectionView::Implementation::updateVisibleItems() {
    auto visibleRows = getVisibleRows();
    std::vector<ItemNumber> newVisibleItems = getItemsInRows(visibleRows);
    std::vector<ItemNumber> toFree;
    std::set_difference(
        visibleItems.begin(), visibleItems.end(),
        newVisibleItems.begin(), newVisibleItems.end(),
        std::back_inserter(toFree)
    );
    freeItems(toFree);
    
    Vector<CollectionViewItem *> newVisibleWidgets;
    Node *innerContainer = owner->getContainer();
    for (ItemNumber itemNumber : newVisibleItems) {
        CollectionViewItem *widget = getWidget(itemNumber);
        if (!widget->getParent()) {
            widget->setAnchorPoint({0, 1});
            widget->setPosition(itemPositions[itemNumber]);
            widget->setContentSize(itemsSizes[itemNumber]);
            widget->setSelected(isItemSelected(itemNumber), false);
            widget->setHighlighted(false);
            innerContainer->addChild(widget);
        }
        newVisibleWidgets.pushBack(widget);
    }
    
    visibleItems = newVisibleItems;
    visibleWidgets = newVisibleWidgets;
}


#pragma mark Support

/// Returns items numbers that located in specified rows
std::vector<CollectionView::ItemNumber> CollectionView::Implementation::getItemsInRows(const std::vector<size_t> &rowsIndices) const {
    if (rowsIndices.empty()) {
        return {};
    }
    
    const size_t startRowNumber = rowsIndices.front();
    const size_t endRowNumber = rowsIndices.back();

    ItemNumber first = rows[startRowNumber];
    ItemNumber end;
    if (endRowNumber + 1 < rows.size()) {
        end = rows[endRowNumber + 1]; //after last in row
    } else {
        end = itemsSizes.size(); //after last item
    }
    std::vector<ItemNumber> ret(end - first);
    std::generate(ret.begin(), ret.end(), Collections::IncrementingSequence<ItemNumber>(first, 1));
    return ret;
}

/// Returns numbers of visible rows
std::vector<size_t> CollectionView::Implementation::getVisibleRows() const {
    if (rowsPositions.empty()) {
        return {};
    }
    
    auto innerContainer = owner->getContainer();
    const float bottom = innerContainer->convertToNodeSpace(owner->convertToWorldSpace(Vec2::ZERO)).y;
    const float top = innerContainer->convertToNodeSpace(owner->convertToWorldSpace(Vec2(0, owner->getViewSize().height))).y;
    
    auto fromIt = std::upper_bound(rowsPositions.rbegin(), rowsPositions.rend(), top);
    auto toIt = std::lower_bound(rowsPositions.rbegin(), rowsPositions.rend(), bottom);
    
    auto startRow = std::distance(fromIt, rowsPositions.rend());
    if (startRow > 0) {
        startRow -= 1;
    }
    auto endRow = std::distance(toIt, rowsPositions.rend());
    
    std::vector<size_t> ret(endRow - startRow);// + 1);
    std::generate(ret.begin(), ret.end(), Collections::IncrementingSequence<size_t>(startRow, 1));
    return ret;
}

size_t CollectionView::Implementation::getItemNumberForIndexPath(const IndexPath &path) const {
    if (path.getSection() < sections.size()) {
        return sections[path.getSection()] + path.getItem();
    }
    return 0;
}

IndexPath CollectionView::Implementation::getIndexPathForItemNumber(size_t itemNumber) const {
    size_t sectionNumber = 0;
    for (size_t sectionStartItemNumber: sections) {
        if (sectionStartItemNumber > itemNumber) {
            break;
        }
        sectionNumber++;
    }
    const size_t sectionStartIdx = sections[sectionNumber];
    return IndexPath(sectionNumber, itemNumber - sectionStartIdx);
}

Rect CollectionView::Implementation::getItemPosition(ItemNumber item) const {
    const auto itemOrigin = itemPositions[item];
    const Size itemSize = itemsSizes[item];
    return Rect(itemOrigin.x, itemOrigin.y - itemSize.height, itemSize.width, itemSize.height);
}


#pragma mark Deque widgets
    
void CollectionView::Implementation::setReuseWidget(CollectionViewItem *widget, const std::string &reuseID) {
    widget->setReuseID(reuseID.c_str());
    widget->stopAllActions();
    reuseTemplates.insert(reuseID, widget);
}

/// Checks if there any widget ready for reuse with specified id
/// If not, create new widget by cloning template with specified id
CollectionViewItem *CollectionView::Implementation::getReusableWidget(const std::string &reuseID) {
    CollectionViewItem *ret = nullptr;
    auto it = freedWidgets.find(reuseID);
        
    if (it != freedWidgets.end()) {
        auto &freed = (*it).second;
        if (!freed.empty()) {
            ret = freed.back();
            ret->retain();
            freed.popBack();
            ret->autorelease();
        }
    }
    
    if (!ret) {
        auto templateIt = reuseTemplates.find(reuseID);
        CC_ASSERT(templateIt != reuseTemplates.end());
        ret = dynamic_cast<CollectionViewItem *>((*templateIt).second->clone());
    }

    return ret;
}
    
CollectionViewItem *CollectionView::Implementation::getWidgetIfVisible(ItemNumber item) {
    auto it = std::lower_bound(visibleItems.begin(), visibleItems.end(), item);
    if (it != visibleItems.end() && *it == item) {
        return visibleWidgets.at( std::distance(visibleItems.begin(), it) );
    }
    return nullptr;
}

CollectionViewItem *CollectionView::Implementation::getWidget(ItemNumber index) {
    auto ret = getWidgetIfVisible(index);
    if (!ret) {
        ret = dataSource->viewForItem(owner, index);
    }
    return ret;
}

/// Remove specified items off screen
/// Mark them as ready for reuse
void CollectionView::Implementation::freeItems(const std::vector<ItemNumber> &toFree) {
    const std::vector<ItemNumber> toFree1 = toFree;
    std::vector<size_t> indices = Collections::getIndices(
         visibleItems.begin(), visibleItems.end(),
         toFree.begin(), toFree.end()
    );
    
    for (size_t index : indices) {
        CollectionViewItem *widget = visibleWidgets.at(index);
        const std::string reuseID = widget->getReuseID();
        if (!reuseID.empty()) {
            if (freedWidgets.find(reuseID) == freedWidgets.end()) {
                freedWidgets.insert({reuseID, Vector<CollectionViewItem *>()});
            }
            
            auto &widgets = freedWidgets.at(reuseID);
            widgets.pushBack(widget);

        }
        widget->removeFromParent();
    }
    
    Collections::eraseIndices(visibleItems, indices);
    Collections::eraseIndices(visibleWidgets, indices);
}

    
#pragma mark Touch
    
CollectionView::ItemNumber CollectionView::Implementation::getItemAtPosition(const Vec2 &pos, bool &contains) {
    if (rowsPositions.empty()) {
        contains = false;
        return -1;
    }
    auto it = std::lower_bound(rowsPositions.rbegin(), rowsPositions.rend(), pos.y);
    size_t number = std::distance(it, rowsPositions.rend());
    if (number > 0) {
        number -= 1;
    }
    auto itemsInRow = getItemsInRows({number});
    for (ItemNumber item : itemsInRow) {
        const auto itemOrigin = itemPositions[item];
        const Size itemSize = itemsSizes[item];
        const Rect rect(itemOrigin.x, itemOrigin.y - itemSize.height, itemSize.width, itemSize.height);
        if (rect.containsPoint(pos)) {
            contains = true;
            return item;
        }
    }
    contains = false;
    return 0;
}
    
void CollectionView::Implementation::selectItem(ItemNumber item,  bool animated) {
    if (isItemSelected(item)) {
        return;
    }
    if (!allowedMultipleSelection && selectedItems.size()) {
        auto prevSelected = selectedItems;
        for (ItemNumber number : prevSelected) {
            deselectItem(number, true);
        }
    }
    
    selectedItems.push_back(item);
    auto widget = getWidgetIfVisible(item);
    if (widget) {
        widget->setSelected(true, animated && widget->isRunning());
    }
    if (delegate) {
        delegate->didSelectItem(owner, item);
    }
}
    
void CollectionView::Implementation::deselectItem(ItemNumber item,  bool animated) {
    auto it = std::find(selectedItems.begin(), selectedItems.end(), item);
    if (it != selectedItems.end()) {
        selectedItems.erase(it);
        
        auto widget = getWidgetIfVisible(item);
        if (widget) {
            widget->setSelected(false, animated);
        }
        if (delegate) {
            delegate->didDeselectItem(owner, item);
        }
    }
}
    
bool CollectionView::Implementation::isItemSelected(ItemNumber item) const {
    if (selectedItems.empty()) {
        return false;
    }
    return std::find(selectedItems.begin(), selectedItems.end(), item) != selectedItems.end();
}
    
    
void CollectionView::Implementation::highlightItem(int touchID, ItemNumber item) {
    auto it = highlightedItems.find(touchID);
    if (it != highlightedItems.end() && (*it).second == item) {
        return;
    }
    unhighlightItem(touchID);
    highlightedItems[touchID] = item;
    auto widget = getWidgetIfVisible(item);
    if (widget) {
        widget->setHighlighted(true);
    }
}
    
void CollectionView::Implementation::unhighlightItem(int touchID) {
    auto it = highlightedItems.find(touchID);
    if (it != highlightedItems.end()) {
        auto widget = getWidgetIfVisible((*it).second);
        if (widget) {
            widget->setHighlighted(false);
        }
        highlightedItems.erase(touchID);
    }
}
    
    
#pragma mark -
#pragma mark Forwarding to implementation
    
CollectionView::CollectionView() {
    pimpl = ImplPtr(new Implementation(this));
    prevScrollPosition = -10;
}

CollectionView::~CollectionView() {
    
}

bool CollectionView::init() {
    ScrollView::init();
    setDirection(ScrollView::Direction::VERTICAL);
    return true;
}

void CollectionView::setDataSource(DataSource *dataSource) {
    pimpl->setDataSource(dataSource);
}

void CollectionView::setDelegate(Delegate *delegate) {
    pimpl->setDelegate(delegate);
}

void CollectionView::onEnter() {
    ScrollView::onEnter();
    scheduleUpdate();
}

void CollectionView::onExit() {
    ScrollView::onExit();
    unscheduleUpdate();
}

void CollectionView::update(float dt) {
    ScrollView::update(dt);
    const float scrollPosition = getContainer()->getPositionY();
    if (fabsf(scrollPosition - prevScrollPosition) > 0.01) {
        prevScrollPosition = scrollPosition;
        pimpl->updateVisibleItems();
    }
}

void CollectionView::reloadData() {
    pimpl->reloadData();
}

void CollectionView::selectItem(ItemNumber item) {
    pimpl->selectItem(item, true);
}

void CollectionView::deselectItem(ItemNumber item) {
    pimpl->deselectItem(item, true);
}

void CollectionView::setReuseTemplateItem(CollectionViewItem *item, const std::string &reuseID) {
    pimpl->setReuseWidget(item, reuseID);
}

CollectionViewItem *CollectionView::getReusableItem(const std::string &reuseID) {
    return pimpl->getReusableWidget(reuseID);
}

CollectionViewItem *CollectionView::getItemIfVisible(ItemNumber number) {
    return pimpl->getWidgetIfVisible(number);
}

void CollectionView::setContentSize(const Size &size) {
    ScrollView::setViewSize(size);
    if (size.height > 0.1f && size.width > 0.1f) {
        pimpl->updatePositions();
    }    
}

void CollectionView::setDefaultItemSize(const Size &size) {
    pimpl->setDefaultItemSize(size);
}

void CollectionView::setInterItemSpacing(const Size &size) {
    pimpl->setInterItemSpacing(size);
}

void CollectionView::setContentMargin(const Margin &margin) {
    pimpl->setContentMargin(margin);
}

void CollectionView::setAlignLeft(bool alignLeft) {
    pimpl->alignLeft = alignLeft;
}

void CollectionView::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags) {
    groupCommand.init(_globalZOrder);
    renderer->addCommand(&groupCommand);
    renderer->pushGroup(groupCommand.getRenderQueueID());
    ScrollView::visit(renderer, parentTransform, parentFlags);
    renderer->popGroup();
}

void CollectionView::onTouchEnded(Touch *touch, Event *unusedEvent) {
    ScrollView::onTouchEnded(touch, unusedEvent);
    pimpl->unhighlightItem(touch->getID());
    
    const auto startLocation = convertToNodeSpace(touch->getStartLocation());
    const auto curLocation = convertToNodeSpace(touch->getLocation());
    if (curLocation.getDistanceSq(startLocation) > 500) {
        return;
    }
    const auto pos = getContainer()->convertToNodeSpace(touch->getLocation());

    bool touched = false;
    ItemNumber item = pimpl->getItemAtPosition(pos, touched);
    if (touched) {
        if (pimpl->isItemSelected(item)) {
            pimpl->deselectItem(item, true);
        } else {
            pimpl->selectItem(item, true);
        }
    }

}
    
bool CollectionView::onTouchBegan(Touch *touch, Event *unusedEvent) {
    if (ScrollView::onTouchBegan(touch, unusedEvent)) {
        const auto pos = getContainer()->convertToNodeSpace(touch->getLocation());
        bool touched = false;
        ItemNumber item = pimpl->getItemAtPosition(pos, touched);
        if (touched) {
            pimpl->highlightItem(touch->getID(), item);
        }
        return true;
    }
    return false;
}

Rect CollectionView::getItemPosition(ItemNumber item) const {
    return pimpl->getItemPosition(item);
}

NS_CC_END

