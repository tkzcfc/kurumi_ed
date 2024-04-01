-- @Author: fangcheng
-- @Date:   2019-06-06 22:08:00
-- @remark: 音频管理器

local AudioManager = class("AudioManager", import(".BaseManager"))

function AudioManager:override_onInit()
    AudioManager.super.override_onInit(self)

    cc.AudioEngine:lazyInit()

    -- 音乐开关
    self.bIsOpenMusic = _MyG.SysSetManager:getProperty("isOpenMusic")
    -- 音效开关
    self.bIsOpenEffect = _MyG.SysSetManager:getProperty("isOpenEffects")
    --背景音量
    self.fMusicVolume = _MyG.SysSetManager:getProperty("fMusicVolume")
    --音效音量
    self.fEffectVolume = _MyG.SysSetManager:getProperty("fEffectVolume")

    -- 背景音乐id
    self.iBackMusicID = nil 
    --背景音乐名称
    self.strBackgroundMusicName = nil
    -- 音效id集合
    self.tSoundIds = {}
end

function AudioManager:playBackMusic(file_name, loop, delay)
    if not self.bIsOpenMusic then return end
    self:stopBackMusic()

    self.strBackgroundMusicName = file_name

    loop = loop or false
    delay = delay or 0

    assert(cc.FileUtils:getInstance():isFileExist(file_name), string.format("AudioManager:找不到背景音乐配置%s的文件:%s", configName, file_name))

    if delay > 0 then
        delayCall(function ()
            self.iBackMusicID = cc.AudioEngine:play2d(file_name, loop, self.fMusicVolume)
        end, delay)
    else
        self.iBackMusicID = cc.AudioEngine:play2d(file_name, loop, self.fMusicVolume)
    end

    return true
end

function AudioManager:stopBackMusic()
    if not self.bIsOpenMusic then return end

    if self.iBackMusicID then
        cc.AudioEngine:stop(self.iBackMusicID)
        self.iBackMusicID = nil
    end
    self.strBackgroundMusicName = nil
end

function AudioManager:getBackMusicName()
    return self.strBackgroundMusicName
end

function AudioManager:pauseBackMusic()
    if not self.bIsOpenMusic then return end

    if self.iBackMusicID then
        cc.AudioEngine:pause(self.iBackMusicID)
    end
end

function AudioManager:resumeBackMusic()
    if not self.bIsOpenMusic then return end

    if self.iBackMusicID then
        cc.AudioEngine:resume(self.iBackMusicID)
    end
end

function AudioManager:playSound(file, loop)
    loop = loop or false
    local id = cc.AudioEngine:play2d(file, loop, self.fEffectVolume)
    cc.AudioEngine:setFinishCallback(id, function (c1, c2)
        self.tSoundIds[c1] = nil
    end)
    self.tSoundIds[id] = file
    return id
end

function AudioManager:stopSound(soundId)
    if not self.bIsOpenEffect then return end

    cc.AudioEngine:stop(soundId)

    self.tSoundIds[soundId] = nil
end

function AudioManager:pauseSound(soundId)
    if not self.bIsOpenEffect then return end

    cc.AudioEngine:pause(soundId)
end

function AudioManager:resumeSound(soundId)
    if not self.bIsOpenEffect then return end

    cc.AudioEngine:resume(soundId)
end

function AudioManager:stopAllSounds()
    if not self.bIsOpenEffect then return end

    for k, v in pairs(self.tSoundIds) do
        self:stopSound(k)
    end
end

function AudioManager:pauseAllSounds()
    if not self.bIsOpenEffect then return end

    for k, v in pairs(self.tSoundIds) do
        self:pauseSound(k)
    end
end

function AudioManager:resumeAllSounds()
    if not self.bIsOpenEffect then return end

    for k, v in pairs(self.tSoundIds) do
        self:resumeSound(k)
    end
end

function AudioManager:turnOnBackMusic(turnOn)
    self.bIsOpenMusic = turnOn
    _MyG.SysSetManager:setProperty("isOpenMusic", self.bIsOpenMusic)
end

function AudioManager:turnOnSound(turnOn)
    self.bIsOpenEffect = turnOn
    _MyG.SysSetManager:setProperty("isOpenEffects", self.bIsOpenEffect)
end

function AudioManager:setBackgroundVolume(volume)
    if volume > 1.0 then volume = 1.0 end
    if volume < 0.0 then volume = 0.0 end

    if self.fMusicVolume == volume then return end

    self.fMusicVolume = volume
    _MyG.SysSetManager:setProperty("fMusicVolume", self.fMusicVolume)

    if self.iBackMusicID then
        cc.AudioEngine:setVolume(self.iBackMusicID, volume)
    end
end

function AudioManager:getBackgroundVolume()
    return self.fMusicVolume
end

function AudioManager:getEffectVolume()
    return self.fEffectVolume
end

function AudioManager:preload(file, func)
    cc.AudioEngine:preload(file, func)
end

function AudioManager:uncache(file)
    cc.AudioEngine:uncache(file)
end

function AudioManager:setEffectVolume(volume)
    if volume > 1.0 then volume = 1.0 end
    if volume < 0.0 then volume = 0.0 end

    if self.fEffectVolume == volume then return end

    self.fEffectVolume = volume
    _MyG.SysSetManager:setProperty("fEffectVolume", self.fEffectVolume)

    for k, v in pairs(self.tSoundIds) do
        if v ~= nil then
            cc.AudioEngine:setVolume(k, volume)
        end
    end
end

return AudioManager
