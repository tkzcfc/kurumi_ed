-- @Author : fc
-- @Date   : 2021-12-31 18:01:10
-- @remark : 冲力

local ImpulseTrack = class("ImpulseTrack", import(".EventTrack"))

ImpulseTrack.NameLang = "GFrameType.IMPULSE_FRAME"
ImpulseTrack.Type = GFrameType.IMPULSE_FRAME


local valueTypeOptions = enum_options("GValueType")

G_SysEventEmitter:on(SysEvent.ON_CHANGE_LANG, function()
    valueTypeOptions = enum_options("GValueType")
end, SysEvent.TAG)

-- @brief 创建新的轨道
function ImpulseTrack:newTrack(cb)
    -- 轨道名称
    local curText = ""

    ------------------------------------------ GEventFrame ------------------------------------------
    -- 最大触发次数,0为一直触发
    local triggerMaxCount = 0
    -- 触发间隔,默认0
    local triggerInterval = 0
    
    ------------------------------------------ GForceFrame ------------------------------------------
    local impulse = {0, 0, 0} -- vec3
    local valueType = 0

    
    local popup = require("app.imgui.Popup").new()
    popup:setWindowFlag(ImGuiWindowFlags_AlwaysAutoResize)
    popup:setGUICallback(function()
        -- 触发次数
        local ok, value = ImGui.InputInt(STR("TriggerMaxCount"), triggerMaxCount)
        if ok then
            triggerMaxCount = value
        end

        -- 触发间隔
        ok, value = ImGui.InputInt(STR("TriggerInterval"), triggerInterval)
        if ok then
            triggerInterval = value
        end
        
        -- 冲力
        ImGui.DragFloat3(STR("Impulse"), impulse, 1)

        -- 值类型
        valueType = Tools:imguiCombo_OneLiner(STR("GValueType"), valueType, valueTypeOptions)
            
        -- name
        ok, value = Tools:imgui_inputText(STR("Name"), curText, 32, ImGuiInputTextFlags_EnterReturnsTrue)
        curText = value
        
        ImGui.Separator()
    
        if ImGui.Button("OK", cc.p(-1, 0)) then
            -- 关闭弹窗
            popup:close()

            local track = {
                name = curText,
                type = self.Type,
                clamp = true,
                frames = {}
            }

            table.insert(track.frames, self:newFrame(triggerMaxCount, triggerInterval, impulse, valueType))

            if cb then
                cb(track)
            end
        end
    end)
    _MyG.PopupManager:addPopup(popup)
end

function ImpulseTrack:newDefaultFrame(trackData)
    return self:newFrame(0, 0, {0, 0, 0})
end

function ImpulseTrack:newFrame(triggerMaxCount, triggerInterval, impulse, valueType)
    local frame = ImpulseTrack.super.newFrame(self, triggerMaxCount, triggerInterval)
    frame.impulse = {x = impulse[1], y = impulse[2], z = impulse[3]}
    frame.valueType = valueType
    return frame
end

local cache_vec3_arr = {0, 0, 0}
function ImpulseTrack:onGUI_FrameCustomTrackInfo(frame, track, curTime, maxTime, onChangePreCB, onChangeFinishCB, context)
    cache_vec3_arr[1] = frame.impulse.x
    cache_vec3_arr[2] = frame.impulse.y
    cache_vec3_arr[3] = frame.impulse.z
    
    -- 冲力
    if ImGui.DragFloat3(STR("Impulse"), cache_vec3_arr, 1) then
        onChangePreCB()
        frame.impulse.x = cache_vec3_arr[1]
        frame.impulse.y = cache_vec3_arr[2]
        frame.impulse.z = cache_vec3_arr[3]
    end

    -- 值类型
    local newValue = Tools:imguiCombo_OneLiner(STR("GValueType"), frame.valueType, valueTypeOptions)
    if newValue ~= frame.valueType then
        onChangePreCB()
        frame.valueType = newValue
    end
end

return ImpulseTrack