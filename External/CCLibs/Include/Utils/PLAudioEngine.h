//
//  PLAudioEngine.h
//  BlackHedgehog
//
//  Created by Mikhail Shulepov on 12/08/15.
//
//
#include <string>

#pragma once

class PLAudioEngine {
public:
    static PLAudioEngine *getInstance();
    
    /**
     * Release the shared Engine object.
     *
     * @warning It must be called before the application exit, or it will lead to memory leaks.
     * @lua destroyInstance
     */
    static void end();
    
    /**
     * Play background music.
     *
     * @param filePath The path of the background music file,or the FileName of T_SoundResInfo.
     * @param loop Whether the background music loop or not.
     * @js playMusic
     * @lua playMusic
     */
    void playBackgroundMusic(const std::string &filePath, bool loop = false);
    
    /**
     * Stop playing background music.
     *
     * @param releaseData If release the background music data or not.As default value is false.
     * @js stopMusic
     * @lua stopMusic
     */
    void stopBackgroundMusic();
    
    /**
     * Pause playing background music.
     * @js pauseMusic
     * @lua pauseMusic
     */
    void pauseBackgroundMusic();
    
    /**
     * Resume playing background music.
     * @js resumeMusic
     * @lua resumeMusic
     */
    void resumeBackgroundMusic();
    
    /**
     * Indicates whether the background music is playing.
     *
     * @return <i>true</i> if the background music is playing, otherwise <i>false</i>.
     * @js isMusicPlaying
     * @lua isMusicPlaying
     */
    bool isBackgroundMusicPlaying();
    
    //
    // properties
    //
    
    /**
     * The volume of the background music within the range of 0.0 as the minimum and 1.0 as the maximum.
     * @js getMusicVolume
     * @lua getMusicVolume
     */
    float getBackgroundMusicVolume() {
        return musicVolume;
    }
    
    /**
     * Set the volume of background music.
     *
     * @param volume must be within the range of 0.0 as the minimum and 1.0 as the maximum.
     * @js setMusicVolume
     * @lua setMusicVolume
     */
    void setBackgroundMusicVolume(float volume);
    
    /**
     * The volume of the effects within the range of 0.0 as the minimum and 1.0 as the maximum.
     */
    float getEffectsVolume() {
        return effectsVolume;
    }
    
    /**
     * Set the volume of sound effects.
     *
     * @param volume must be within the range of 0.0 as the minimum and 1.0 as the maximum.
     */
    void setEffectsVolume(float volume) {
        effectsVolume = volume;
    }
    
    //
    // for sound effects
    
    /**
     * Play sound effect with a file path, pitch, pan and gain.
     *
     * @param filePath The path of the effect file.
     * @param loop Determines whether to loop the effect playing or not. The default value is false.
     */
    void playEffect(const std::string &filePath, bool loop = false);
    
    /**
     * Preload a compressed audio file.
     *
     * @param filePath The path of the effect file.
     * @js NA
     */
    void preload(const std::string &filePath);
    
    /**
     * Unload the preloaded effect from internal buffer.
     *
     * @param filePath The path of the effect file.
     */
    void unload(const std::string &filePath);
    
    void pauseAll();
    void resumeAll();
    
private:
    PLAudioEngine();
    ~PLAudioEngine();
    
    float effectsVolume = 1.0f;
    float musicVolume = 1.0f;
    
    int backgroundMusicId = -1;
    std::string backgroundMusicFile;
    bool loopBackgroundMusic = false;
};
