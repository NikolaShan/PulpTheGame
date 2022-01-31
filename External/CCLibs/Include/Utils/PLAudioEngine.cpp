//
//  PLAudioEngine.cpp
//  BlackHedgehog
//
//  Created by Mikhail Shulepov on 12/08/15.
//
//

#include "PLAudioEngine.h"
#include "base/CCMacros.h"
#include "AppUtils.h"

PLAudioEngine *PLAudioEngine::getInstance() {
    static auto instance = new PLAudioEngine;
    return instance;
}

PLAudioEngine::PLAudioEngine() {
    
}

PLAudioEngine::~PLAudioEngine() {
    
}

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#include "AudioEngine.h"
using namespace cocos2d::experimental;

void PLAudioEngine::end() {
    AudioEngine::end();
}

void PLAudioEngine::playBackgroundMusic(const std::string &filePath, bool loop) {
    stopBackgroundMusic();
    backgroundMusicFile = filePath;
    loopBackgroundMusic = loop;
    if (musicVolume > FLT_EPSILON) {
        backgroundMusicId = AudioEngine::play2d(filePath, loop, musicVolume);
    }
}

void PLAudioEngine::stopBackgroundMusic() {
    if (backgroundMusicId >= 0) {
        AudioEngine::stop(backgroundMusicId);
    }
    backgroundMusicFile = "";
    backgroundMusicId = -1;
}

void PLAudioEngine::pauseBackgroundMusic() {
    if (backgroundMusicId >= 0) {
        AudioEngine::pause(backgroundMusicId);
    }
}

void PLAudioEngine::resumeBackgroundMusic() {
    if (backgroundMusicId >= 0) {
        AudioEngine::resume(backgroundMusicId);
    }
}

bool PLAudioEngine::isBackgroundMusicPlaying() {
    if (backgroundMusicId >= 0) {
        const AudioEngine::AudioState state = AudioEngine::getState(backgroundMusicId);
        return state == AudioEngine::AudioState::PLAYING;
    }
    return false;
}

void PLAudioEngine::setBackgroundMusicVolume(float volume) {
    musicVolume = volume;
    if (backgroundMusicId >= 0) {
        AudioEngine::setVolume(backgroundMusicId, volume);
    } else if (volume > FLT_EPSILON && !backgroundMusicFile.empty() && !isBackgroundMusicPlaying()) {
        const std::string forceCopyFileName = backgroundMusicFile;
        playBackgroundMusic(forceCopyFileName, loopBackgroundMusic);
    }
}

//
// for sound effects

void PLAudioEngine::playEffect(const std::string &filePath, bool loop) {
    if (effectsVolume > FLT_EPSILON) {
        AudioEngine::play2d(filePath, loop, effectsVolume);
    }
}

void PLAudioEngine::preload(const std::string &filePath) {
    AudioEngine::preload(filePath);
}

void PLAudioEngine::unload(const std::string &filePath) {
    AudioEngine::uncache(filePath);
}

void PLAudioEngine::pauseAll() {
    AudioEngine::pauseAll();
}

void PLAudioEngine::resumeAll() {
    AudioEngine::resumeAll();
}

#else

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

void PLAudioEngine::end() {
    SimpleAudioEngine::end();
}

void PLAudioEngine::playBackgroundMusic(const std::string &filePath, bool loop) {
    SimpleAudioEngine::getInstance()->playBackgroundMusic(filePath.c_str(), loop);
}

void PLAudioEngine::stopBackgroundMusic() {
    SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void PLAudioEngine::pauseBackgroundMusic() {
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void PLAudioEngine::resumeBackgroundMusic() {
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

bool PLAudioEngine::isBackgroundMusicPlaying() {
    return SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}

void PLAudioEngine::setBackgroundMusicVolume(float volume) {
    if (AppUtils::backgroundMusicShouldBeSilenced()) {
        musicVolume = 0.0f;
    } else {
        musicVolume = volume;
    }
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(musicVolume);
}

//
// for sound effects

void PLAudioEngine::playEffect(const std::string &filePath, bool loop) {
    if (effectsVolume > FLT_EPSILON) {
        SimpleAudioEngine::getInstance()->playEffect(filePath.c_str(), loop);
    }
}

void PLAudioEngine::preload(const std::string &filePath) {
    SimpleAudioEngine::getInstance()->preloadEffect(filePath.c_str());
}

void PLAudioEngine::unload(const std::string &filePath) {
    SimpleAudioEngine::getInstance()->unloadEffect(filePath.c_str());
}

void PLAudioEngine::pauseAll() {
    auto audioEngine = SimpleAudioEngine::getInstance();
    audioEngine->pauseBackgroundMusic();
    audioEngine->pauseAllEffects();
}

void PLAudioEngine::resumeAll() {
    auto audioEngine = SimpleAudioEngine::getInstance();
    audioEngine->resumeBackgroundMusic();
    audioEngine->resumeAllEffects();
}

#endif
