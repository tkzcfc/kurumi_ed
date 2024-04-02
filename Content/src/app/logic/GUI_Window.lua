-- @Author : fc
-- @Date   : 2021-9-2 10:14:04
-- @remark : 

cc.exports.WinTag = enum {
    "IMGUI_DEMO",
    "ROLE",
    "GLOBAL_SCALE",
    "FONT_SETTING",
    "PROJECT",
    "LOG",
    "CONTENT",
    "PROPERTY",
    "NODETREE",
    "TIMELINE",
    "PREVIEW"
}

local WindowManager = require("app.imgui.WindowManager").new()

_MyG.WindowManager = WindowManager

G_SysEventEmitter:on(SysEvent.ON_GUI, function()
    WindowManager:onGUI()
end, SysEvent.TAG)

local win

-- ImGui Demo
win = require("app.imgui.window.ImGuiDemo").new("ImGui Demo", true)
win:setWindowVisible(false)
WindowManager:add(win, WinTag.IMGUI_DEMO)

-- GlobalScaleWindow
win = require("app.imgui.window.GlobalScaleWindow").new("Global Scale", true)
win:setWindowVisible(false)
WindowManager:add(win, WinTag.GLOBAL_SCALE)

-- FontSetting
win = require("app.imgui.window.FontSetting").new("Font", true)
win:setWindowVisible(false)
WindowManager:add(win, WinTag.FONT_SETTING)

-- Project
win = require("app.imgui.window.Project").new()
WindowManager:add(win, WinTag.PROJECT)

-- Log
win = require("app.imgui.window.Log").new()
WindowManager:add(win, WinTag.LOG)

-- Content
win = require("app.imgui.window.Content").new()
WindowManager:add(win, WinTag.CONTENT)


-- Property
win = require("app.imgui.window.Property").new()
WindowManager:add(win, WinTag.PROPERTY)

-- NodeTree
win = require("app.imgui.window.NodeTree").new()
WindowManager:add(win, WinTag.NODETREE)

-- Timeline
win = require("app.imgui.window.Timeline").new()
WindowManager:add(win, WinTag.TIMELINE)

-- Role
win = require("app.imgui.window.Role").new()
win:setWindowVisible(false)
WindowManager:add(win, WinTag.ROLE)

-- Preview
win = require("app.imgui.window.Preview").new()
win:setWindowVisible(false)
WindowManager:add(win, WinTag.PREVIEW)