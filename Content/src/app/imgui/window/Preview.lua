
local Preview = class("Preview", require("app.imgui.Window"))

local spriteFrameCache =  cc.SpriteFrameCache:getInstance()
local textureCache = cc.Director:getInstance():getTextureCache()

function Preview:ctor()
    Preview.super.ctor(self, self.__cname, true)
    G_SysEventEmitter:on(SysEvent.ON_PREVIEW_TEXTURE, function(imgPath, isPlist, plistFile)
        self.imgPath = imgPath
        self.isPlist = isPlist
        self.plistFile = plistFile
        self.enablePreview = true
        self:setWindowVisible(true)
    end, self)

    self:setWindowFlags(ImGuiWindowFlags_HorizontalScrollbar)
end

function Preview:onGUI_Window()
    if self.enablePreview then
        if self.isPlist then
            if not spriteFrameCache:getSpriteFrame(self.imgPath) then
                spriteFrameCache:addSpriteFrames(self.plistFile)
            end

            local frame = spriteFrameCache:getSpriteFrame(self.imgPath)
            if frame then
                local size = frame:getOriginalSize()
                ImGuiPresenter:getInstance():image(frame, {x = size.width, y = size.height})
            else
                self.enablePreview = false
            end
        else
            local texture = textureCache:addImage(self.imgPath)
            if texture then
                local width = texture:getPixelsWide()
                local height = texture:getPixelsHigh()
                ImGuiPresenter:getInstance():image(texture, {x = width, y = height})
            else
                self.enablePreview = false
            end
        end
    else
        ImGui.Text("no texture")
    end
end

return Preview