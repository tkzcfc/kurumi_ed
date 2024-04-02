
local Timeline = class("Timeline", require("app.imgui.Window"))

function Timeline:ctor()
    Timeline.super.ctor(self, self.__cname, true)
    self.sequentity = CSequentity()
    self.sequentity:setRange(0, 100)
end

function Timeline:onGUI()
    Tools:BeginWindow_NoClose(self.winName, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar)
    local count = G_SysEventEmitter:emit(SysEvent.ON_TIMELINE_GUI)
    if count < 1 then
        self.sequentity:onGUI()
    end
    ImGui.End()
end

return Timeline