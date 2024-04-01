
local Property = class("Property", require("app.imgui.Window"))

function Property:ctor()
    Property.super.ctor(self, self.__cname, true)
end

function Property:onGUI()
    Tools:BeginWindow_NoClose(self.winName, ImGuiWindowFlags_NoMove)

    ImGui.End()
end

return Property