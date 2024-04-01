
local DocumentManager = require("app.imgui.DocumentManager")
local Project = class("Project", require("app.imgui.Window"))

function Project:ctor()
    Project.super.ctor(self, self.__cname, true)
    self.documentManager = DocumentManager.new(self.__cname)

    local content

    content = require("app.document.ProjContent").new("asset", _MyG.ProjectAssetManager)
    self.documentManager:addDocument(content)

    -- content = require("app.document.CocosContent").new("cocos", _MyG.CocosAssetManager)
    -- self.documentManager:addDocument(content)

    content = require("app.document.GameResContent").new("game", _MyG.GameAssetManager)
    self.documentManager:addDocument(content)

    content = require("app.document.WidgetContent").new()
    self.documentManager:addDocument(content)
end

function Project:onGUI()
    Tools:BeginWindow_NoClose(self.winName)

    self.documentManager:onGUI()

    ImGui.End()
end

return Project