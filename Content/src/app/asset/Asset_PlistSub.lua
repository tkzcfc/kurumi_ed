-- @Author: fangcheng
-- @Date:   2020-07-02 21:14:25
-- @remark: 

local Asset_File = import(".Asset_File")
local Asset_PlistSub = class("Asset_PlistSub", Asset_File)


----------------------------------------------------------------------------------------------------------------

-- @brief 获取生成缓存路径(生成全路径)
local function genCachePath_1(plistFile, pngName)
	-- 缓存路径
	local baseName = string.gsub(plistFile, ".plist$", "")
	baseName = G_Helper.fmtPath(baseName)

	-- 调用saveToFile时候，会在此路径前面加上 cc.FileUtils:getInstance():getWritablePath(),所以此处写相对路径
	return ".cache_plist/" .. baseName .. "/" .. pngName
end

-- @brief 获取生成缓存路径(生成路径md5.png)
local function genCachePath_2(plistFile, pngName)
	-- 缓存路径
	local baseName = string.gsub(plistFile, ".plist$", "")
	baseName = G_Helper.fmtPath(baseName)

	baseName = baseName .. "/" .. pngName

	-- 调用saveToFile时候，会在此路径前面加上 cc.FileUtils:getInstance():getWritablePath(),所以此处写相对路径
	return ".cache_plist/" .. G_Helper.md5(baseName) .. ".png"
end

----------------------------------------------------------------------------------------------------------------


function Asset_PlistSub:ctor(plistFile, textureName, assetManager)
	self.textureName = textureName
	Asset_PlistSub.super.ctor(self, plistFile, assetManager)
end

function Asset_PlistSub:init(fullPath)
	fullPath = G_Helper.fmtPath(fullPath)
	self.property.extension = G_Helper.getExtension(fullPath)
	self.property.thumbnailTexture 		= EditorIconContent:get(EditorIcon.ICON_TEXTURE)
	self.property.fullPath = fullPath
	self.property.relativePath = _MyG.Functions:getRelativePath(fullPath)


	self.property.uuid = G_Helper.md5(fullPath .. self.textureName)
	self.property.resType 		= Asset.ResType.PLIST_SUBGRAPH
	self.property.textureName 	= self.textureName
	self.property.showName 		= self.textureName
	self.property.isPlist 		= true

	-- saveToFile 函数只能传入相对路径,此处没有加 WritablePath
	self.saveToFileName = genCachePath_1(self.property.relativePath, self.textureName)
	-- 在读取图片时加上 WritablePath
	self.cacheTextureName = cc.FileUtils:getInstance():getWritablePath() .. self.saveToFileName

	if cc.FileUtils:getInstance():isFileExist(self.cacheTextureName) then
		self.genTextureTag = true
	end

	local baseName = G_Helper.getLastName(self.cacheTextureName)
	self.cacheTextureDirName = string.sub(self.cacheTextureName, 1, #self.cacheTextureName - #baseName)
end


local cache = {}

local textureCache = cc.Director:getInstance():getTextureCache()

function Asset_PlistSub:_onItemHovered()
	local textureName = self.cacheTextureName

	if self.genTextureTag then
		local texture = textureCache:addImage(textureName)
		if texture then
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
	else
		self.genTextureTag = true

		local texture = textureCache:getTextureForKey(textureName)
		if not texture then
			os.mkdirpath(self.cacheTextureDirName)

			cc.SpriteFrameCache:getInstance():addSpriteFrames(self.property.relativePath)

			local sp = cc.Sprite:createWithSpriteFrameName(self.property.textureName)
			sp:setAnchorPoint(VecZero)
			sp:setPosition(0, 0)
			
			local size = sp:getContentSize()
			
			local renderTexture = cc.RenderTexture:create(size.width, size.height)
			
			-- render
			renderTexture:beginWithClear(0,0,0,0)
			sp:visit()
			renderTexture:endToLua()
			
			renderTexture:saveToFile(self.saveToFileName, cc.IMAGE_FORMAT_PNG, true)
		end
	end
end

function Asset_PlistSub:isGenTexture()
	return self.genTextureTag
end

function Asset_PlistSub:getGenTextureName()
	return self.saveToFileName
end

return Asset_PlistSub
