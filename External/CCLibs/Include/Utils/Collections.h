//
// Created by Mike on 26.04.14.
//



#ifndef __Collections_H_
#define __Collections_H_

#include <stdlib.h>
#include "base/CCValue.h"
#include "base/CCVector.h"

namespace Collections {
    
    cocos2d::ValueMap &getOrInsertValueMap(cocos2d::ValueMap &src, const std::string &key);
    void mergeValueMap(cocos2d::ValueMap &to, const cocos2d::ValueMap &from);
    bool valueMapGetBool(const cocos2d::ValueMap &map, const std::string &key, bool defaultValue);
    int valueMapGetInt(const cocos2d::ValueMap &map, const std::string &key, int defaultValue);
    std::string valueMapGetString(const cocos2d::ValueMap &map, const std::string &key);
    std::string valueMapGetString(const cocos2d::ValueMap &map, const std::string &key, const std::string &defaultValue);

    std::vector<std::string> valueVectorToStringVector(const cocos2d::ValueVector &vv);
    
    const cocos2d::Value &getValueForKeyPath(const cocos2d::ValueMap &target, const std::string &keyPath);
    const cocos2d::Value &getValueForKeyPath(const cocos2d::ValueMap &target, const std::vector<std::string> &keyPath);
    
    template<typename T, typename U>
    size_t indexOf(T begin, T end, const U &value) {
        auto it = std::find(begin, end, value);
        if (it != end) {
            return std::distance(begin, it);
        }
        return -1;
    }
    
    
    template <typename T>
    class IncrementingSequence {
    public:
        IncrementingSequence(T pFrom, T pDelta) : value(pFrom), delta(pDelta) {}
        T operator() () {
            T ret = value;
            value += delta;
            return ret;
        }
    private:
        T value;
        T delta;
    };
    
    /**
     * return indices of items [itemsBegin, itemsEnd] in collection [srcBegin, srcEnd]
     * Items and Collection should be sorted in the same way
     */
    template <typename IteratorColl, typename IteratorItems>
    std::vector<size_t> getIndices(IteratorColl colBegin, IteratorColl colEnd, IteratorItems itemsBegin, IteratorItems itemsEnd) {
        std::vector<size_t> ret;
        auto colIt = colBegin;
        auto itemsIt = itemsBegin;
        size_t index = 0;
        while (colIt != colEnd && itemsIt != itemsEnd) {
            if (*colIt == *itemsIt) {
                ret.push_back(index);
                ++colIt;
                ++itemsIt;
            } else {
                ++colIt;
            }
            ++index;
        }
        return ret;
    }
    
    /* can't assume copy elision, don't pass-by-value */
    template<typename Iter>
    Iter removeIndices(Iter begin, Iter end, std::vector<size_t> &indicesToRemove) {
        if (indicesToRemove.empty()) {
            return end;
        }
        std::sort(indicesToRemove.begin(), indicesToRemove.end());
        
        auto removeIndicesIter = indicesToRemove.begin();
        auto removeIndicesEnd = indicesToRemove.end();
        
        size_t nextToRemove = *removeIndicesIter;
        size_t currentIndex = nextToRemove;
        auto result = begin + currentIndex;
        auto readIter = result;
        
        while (readIter != end) {
            if (currentIndex != nextToRemove) {
                std::swap(*result, *readIter);
                ++result;
            } else {
                ++removeIndicesIter;
                if (removeIndicesIter == removeIndicesEnd) {
                    nextToRemove = 0;
                } else {
                    nextToRemove = *removeIndicesIter;
                }
            }
            ++readIter;
            ++currentIndex;
        }
        
        return result;
    }
    
    template<typename T>
    void eraseIndices(T &vec, std::vector<size_t> &indicesToRemove) {
        auto it = removeIndices(vec.begin(), vec.end(), indicesToRemove);
        vec.erase(it, vec.end());
    }
    
    template<typename T>
    void eraseIndices(cocos2d::Vector<T> &vec, std::vector<size_t> &indicesToRemove) {
        if (indicesToRemove.size() > 0) {
            auto it = removeIndices(vec.begin(), vec.end(), indicesToRemove);
            vec.erase(it, vec.end());
        }
    }
    
    template <class InputIterator> InputIterator
    randomElement(InputIterator first, InputIterator last) {
        typename std::iterator_traits<InputIterator>::difference_type distance = std::distance(first, last);
        InputIterator result = first;
        if (distance > 1) {
            srand(static_cast<unsigned int>(time(nullptr)));
            std::advance( result, rand() % distance );
        }
        return result;
    }
};


#endif //__Collections_H_
