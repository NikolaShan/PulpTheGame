//
//  GameManager.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 20/07/15.
//
//

#include "GameManager.h"

#include "platform/CCFileUtils.h"
#include "base/CCUserDefault.h"

#include "Utils/StringUtils.h"
#include "Utils/GuiUtils.h"
#include "Utils/FilePath.h"
#include "../Resources/Resources.h"
#include "CrashlyticsTracker.h"

#include "JsonLevelParser.h"

using namespace cocos2d;

GameManager *GameManager::getInstance() {
    static GameManager *instance = new GameManager;
    return instance;
}

GameManager::GameManager() {
    const ValueVector colorSchemeData = FileUtils::getInstance()->getValueVectorFromFile(R::COLOR_SCHEME_PLIST);
    for (auto &colorInfo: colorSchemeData) {
        const ValueVector &vv = colorInfo.asValueVector();
        SegmentColorInfo segColorInfo = {
            GuiUtils::getColorRGBAFromString(vv.at(0).asString()),
            GuiUtils::getColorRGBAFromString(vv.at(1).asString())
        };
        colorScheme.push_back(segColorInfo);
    }
    
    remapLevelsFirst();
}

int GameManager::getLevelsCount() const {
    return R::LEVELS_COUNT;
}

LevelDefinition GameManager::getLevel(int number) const {
    CrashlyticsTracker::setInt("Level", number);
    const int remappedNumber = levelsRemap[number];
    CrashlyticsTracker::setInt("Remapped Level", number);
    CCLOG("Load level %d (%d.json)", number, remappedNumber);
    
    const std::string fileName = ::StringUtils::toString(remappedNumber) + ".json";
    const std::string filePath = FilePath::appendPathComponent("Levels", fileName);
    const std::string jsonData = FileUtils::getInstance()->getStringFromFile(filePath);
    auto level = JsonLevelParser().parse(jsonData);
    level.number = number;
    return level;
}

LevelDefinition GameManager::getRandomLevel() const {
    int number = 1 + std::rand() % getLevelsCount();
    static std::vector<int> excludedLevels = {36,37,39,41,42,43,44,45,49,50,54,55,56,58,59,61,65,77,80,84,85,86,87,90,91,92,95,98,99,100,103,115,127,137,139,161,165,169,171,178,212,213,218,223,231,241,250,262,263,266,269,273,275,277,278,279,284,286,288,289,290,294,296,299,301,307,308,309,312,313,321,323,327,329,330,343,344,347,350};
    while (std::find(excludedLevels.begin(), excludedLevels.end(), number) != excludedLevels.end()) {
        number = 1 + std::rand() % getLevelsCount();
    }
    return getLevel(number);
}

int GameManager::getCurrentLevelNumber() const {
    return UserDefault::getInstance()->getIntegerForKey("PramataYou", 1);
}

void GameManager::setCurrentLevel(int number) {
    if (number <= getLevelsCount()) {
        UserDefault::getInstance()->setIntegerForKey("PramataYou", number);
        UserDefault::getInstance()->setIntegerForKey("PLAttemptsCountN", 0);
        UserDefault::getInstance()->flush();
    }
}

void GameManager::setLevelCompleted(int number) {
    if (number > getCompletedLevelsCount()) {
        UserDefault::getInstance()->setIntegerForKey("HuricainPsi", number);
        UserDefault::getInstance()->flush();
    }
}

int GameManager::getCompletedLevelsCount() const {
    return UserDefault::getInstance()->getIntegerForKey("HuricainPsi", 0);
}

bool GameManager::canPlayLevel(int number) const {
    return number <= getCompletedLevelsCount() + 1 && number <= getLevelsCount();
}

void GameManager::incrementAttemptsForCurrentLevel() {
    UserDefault::getInstance()->setIntegerForKey("PLAttemptsCountN", getAttemptsCountForCurrentLevel() + 1);
}

int GameManager::getAttemptsCountForCurrentLevel() const {
    return UserDefault::getInstance()->getIntegerForKey("PLAttemptsCountN", 0);
}

int GameManager::getAvailableLevelSkips() const {
    return UserDefault::getInstance()->getIntegerForKey("IknPouyRu", 0);
}

void GameManager::setAvailableLevelSkips(int count) {
    UserDefault::getInstance()->setIntegerForKey("IknPouyRu", std::max(0, count));
}

int GameManager::getEndlessModeHighscore() const {
    return UserDefault::getInstance()->getIntegerForKey("dKuehfOf", 0);
}

void GameManager::setEndlessModeScore(int score) {
    if (score > getEndlessModeHighscore()) {
        UserDefault::getInstance()->setIntegerForKey("dKuehfOf", score);
    }
}

//insert levels 101-147 into middle
void GameManager::remapLevelsFirst() {
    int remapFrom = UserDefault::getInstance()->getIntegerForKey("LvlRemap", -1);
    if (remapFrom < 0) {
        remapFrom = getCompletedLevelsCount();
        UserDefault::getInstance()->setIntegerForKey("LvlRemap", remapFrom);
        UserDefault::getInstance()->flush();
    }
    std::vector<std::vector<int>> intervals;
    if (remapFrom < 30) {
        intervals = {
            {1, 29}, {101,  110}, {30, 35}, {116, 142}, {36, 50},
            {111, 115}, {51, 60}, {146, 147}, {61, 65}, {143, 145}, {66, 100}
        };
    } else if (remapFrom < 100) {
        intervals = {
            {1, remapFrom},
            {101, 142},
            {remapFrom + 1, 100}
        };
    }
    
    levelsRemap.push_back(0);
    
    int maxLevel = 1;
    for (const auto &interval: intervals) {
        const int from = interval[0];
        const int to = interval[1];
        if (from <= to) {
            for (int i = from; i <= to; ++i) {
                levelsRemap.push_back(i);
            }
            maxLevel = std::max(maxLevel, to);
        }
    }
    
    const int totalLevelsCount = getLevelsCount();
    CCASSERT(levelsRemap.size() - 1 <= totalLevelsCount, "Count of remapped levels more than total amount of levels");
    for (int i = maxLevel + 1; i <= totalLevelsCount; ++i) {
        levelsRemap.push_back(i);
    }
    
    //for (int i = 0; i < levelsRemap.size(); ++i) {
    //    cocos2d::log("%d -> %d", i, levelsRemap[i]);
    //}
}