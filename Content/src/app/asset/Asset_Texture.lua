-- @Author: fangcheng
-- @Date:   2020-04-11 14:29:16
-- @Description: 


local Asset_File = import(".Asset_File")
local Asset_Texture = class("Asset_Texture", Asset_File)

local textureCache = cc.Director:getInstance():getTextureCache()

function Asset_Texture:init(fullPath)
	Asset_Texture.super.init(self, fullPath)
	
	self.property.thumbnailTexture 		= EditorIconContent:get(EditorIcon.ICON_TEXTURE)
end


local cache = {}
function Asset_Texture:_onItemHovered()
	local fullPath = self.property.fullPath

	local texture = textureCache:addImage(fullPath)
	if texture == nil then
		return
	end

	ImGui.BeginTooltip()

	local width = texture:getPixelsWide()
	local height = texture:getPixelsHigh()
	ImGui.Text(string.format("%d*%d", width, height))

	local maxValue = math.max(width, height)
	if maxValue > 500 then
		local scale = 500 / maxValue
		width = width * scale
		height = height * scale
	end
	ImGuiPresenter:getInstance():image(texture, {x = width, y = height})

	ImGui.EndTooltip()
end

return Asset_Texture
