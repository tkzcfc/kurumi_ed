-- @Author : fc
-- @Date   : 2021-12-31 15:08:54
-- @remark : 力施加轨道


local ForceTrack = class("ForceTrack", import(".EventTrack"))


ForceTrack.NameLang = "GFrameType.FORCE_FRAME"
ForceTrack.Type = GFrameType.FORCE_FRAME


local valueTypeOptions = enum_options("GValueType")

G_SysEventEmitter:on(SysEvent.ON_CHANGE_LANG, function()
    valueTypeOptions = enum_options("GValueType")
end, SysEvent.TAG)

-- @brief 创建新的轨道
function ForceTrack:newTrack(cb)
    -- 轨道名称
    local curText = ""

    ------------------------------------------ GEventFrame ------------------------------------------
    -- 最大触发次数,0为一直触发
    local triggerMaxCount = 0
    -- 触发间隔,默认0
    local triggerInterval = 0
    
    ------------------------------------------ GForceFrame ------------------------------------------
    local force = {0, 0, 0} -- vec3
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
        
        -- 力
        ImGui.DragFloat3(STR("Force"), force, 1)

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

            table.insert(track.frames, self:newFrame(triggerMaxCount, triggerInterval, force, valueType))

            if cb then
                cb(track)
            end
        end
    end)
    _MyG.PopupManager:addPopup(popup)
end

function ForceTrack:newDefaultFrame(trackData)
    return self:newFrame(0, 0, {0, 0, 0})
end

function ForceTrack:newFrame(triggerMaxCount, triggerInterval, force, valueType)
    local frame = ForceTrack.super.newFrame(self, triggerMaxCount, triggerInterval)
    frame.force = {x = force[1], y = force[2], z = force[3]}
    frame.valueType = valueType
    return frame
end

local cache_vec3_arr = {0, 0, 0}
function ForceTrack:onGUI_FrameCustomTrackInfo(frame, track, curTime, maxTime, onChangePreCB, onChangeFinishCB, context)
    cache_vec3_arr[1] = frame.force.x
    cache_vec3_arr[2] = frame.force.y
    cache_vec3_arr[3] = frame.force.z
    
    -- 力
    if ImGui.DragFloat3(STR("Force"), cache_vec3_arr, 1) then
        onChangePreCB()
        frame.force.x = cache_vec3_arr[1]
        frame.force.y = cache_vec3_arr[2]
        frame.force.z = cache_vec3_arr[3]
    end
    -- 值类型
    local newValue = Tools:imguiCombo_OneLiner(STR("GValueType"), frame.valueType, valueTypeOptions)
    if newValue ~= frame.valueType then
        onChangePreCB()
        frame.valueType = newValue
    end
end

return ForceTrack