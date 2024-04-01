
local Content = class("Content", require("app.imgui.Window"))

local centerDocumentManager = _MyG.CenterDocumentManager

function Content:ctor()
    Content.super.ctor(self, self.__cname, true)

    _MyG.CenterWindow = self

    self.cache_isContainMouse = false
    self.cache_ceontex_w = 0
    self.cache_ceontex_h = 0

    local render = axui.Layout:create()
    render:setClippingType(1)
    render:setAnchorPoint(cc.p(0, 1))
    _MyG.MainScene.rootNode:addChild(render)

    self.render = render
    self.senderSize = cc.size(0, 0)

    self.rootNode = cc.Node:create()
    render:addChild(self.rootNode, 1)

    self.renderGuiShow = false
    self.renderCanShow = true

    self:initEventDispatcher()
end

function Content:onGUI()
    --Tools:BeginWindow_NoClose(self.winName, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground)
    Tools:BeginWindow_NoClose(self.winName, ImGuiWindowFlags_NoBackground)

    if centerDocumentManager:count() > 0 then
        self.renderGuiShow = true
        self:updateRenderSize()
        self:updateContainStatus()
        centerDocumentManager:onGUI()
    else
        self.renderGuiShow = false
    end

    self.render:setVisible(self.renderGuiShow and self.renderCanShow)

    ImGui.End()
end

function Content:changeEventDispatcher(rootNode)
    if rootNode.isTouchEnabled and rootNode:isTouchEnabled() then
        rootNode:setTouchEnabled(false)
        rootNode:setEventDispatcher(self.eventDispatcher)
        rootNode:setTouchEnabled(true)
    else
        rootNode:setEventDispatcher(self.eventDispatcher)
    end

    local children = rootNode:getChildren()
    if #children < 1 then
        return
    end

    for k,v in pairs(children) do
        self:changeEventDispatcher(v)
    end
end

function Content:changeEventDispatcherSelf(rootNode)
    if rootNode.isTouchEnabled and rootNode:isTouchEnabled() then
        rootNode:setTouchEnabled(false)
        rootNode:setEventDispatcher(self.eventDispatcher)
        rootNode:setTouchEnabled(true)
    else
        rootNode:setEventDispatcher(self.eventDispatcher)
    end
end

function Content:isContainMouse()
    return self.cache_isContainMouse
end

function Content:checkMouseIsContain()
    local pos = _MyG.MouseEventDispatcher:getCursorPos()
    return Tools:isInRect(self.render, pos.x, pos.y)
end


function Content:updateRenderSize()
    local edContext = _MyG.edContext
    local style = ImGui.GetStyle()

    local viewSize = _MyG.edContext:getMainViewportSize()
    local viewPos = _MyG.edContext:getMainViewportPos()

    local pos = ImGui.GetWindowPos()
    local size = cc.size(ImGui.GetWindowWidth(), ImGui.GetWindowHeight())

    pos.x = pos.x - viewPos.x
    pos.y = pos.y - viewPos.y

    pos.y = viewSize.y - pos.y

    if size.height < 20 or size.width < 20 then
        self.renderCanShow = false
        return
    else
        self.renderCanShow = true
    end

    self.senderSize = size

    self.render:setPosition(pos)
    self.render:setContentSize(size)

    if math.abs(size.width - self.cache_ceontex_w) > 0.0001 or math.abs(size.height - self.cache_ceontex_h) > 0.0001 then
        self.cache_ceontex_w = size.width
        self.cache_ceontex_h = size.height
        self:onContentSizeChange()
    end
end

function Content:updateBGGrid()
    local gridRender = self.gridRender
    if gridRender == nil then
        gridRender = cc.Sprite:create("Res/grid.png")
        gridRender:getTexture():setTexParameters(
            3, -- minFilter = LINEAR
            3, -- magFilter = LINEAR
            0, -- wrapS = REPEAT
            0 -- wrapT = REPEAT
        )
        self.render:addChild(gridRender, 0)

        gridRender.cache_width = 0
        gridRender.cache_height = 0

        self.gridRender = gridRender
    end

    if gridRender.cache_width < self.senderSize.width or gridRender.cache_height < self.senderSize.height then
        gridRender:setPosition(self.senderSize.width * 0.5, self.senderSize.height * 0.5)

        local textureRect = cc.rect(0, 0, self.senderSize.width, self.senderSize.height)
        gridRender:setTextureRect(textureRect)

        gridRender.cache_width = self.senderSize.width
        gridRender.cache_height = self.senderSize.height
    end
end

function Content:addChild(node, z)
    self.rootNode:addChild(node, z or 0)
end

function Content:onContentSizeChange()
    self.rootNode:setPosition(self.cache_ceontex_w * 0.5, self.cache_ceontex_h * 0.5)
    self:updateBGGrid()
end

function Content:updateContainStatus()
    self.cache_isContainMouse_valid = true

    if not ImGui.IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) then
        self.cache_isContainMouse =false
        return false
    end

    local style = ImGui.GetStyle()
    local canvas_pos = ImGui.GetCursorScreenPos()
    local canvas_size = ImGui.GetContentRegionAvail()
    local mousePos = ImGui.GetIO().MousePos
    local mouse_pos_in_canvas = cc.p(mousePos.x - canvas_pos.x, mousePos.y - canvas_pos.y)

    if mouse_pos_in_canvas.x >= 0 and mouse_pos_in_canvas.y >= 0 and mouse_pos_in_canvas.x <= canvas_size.x and mouse_pos_in_canvas.y <= canvas_size.y then
        self.cache_isContainMouse = true
    else
        self.cache_isContainMouse = false
    end
    self.cache_mouse_pos_in_canvas = mouse_pos_in_canvas

end


function Content:initEventDispatcher()
    self.cache_MouseDownStatus = {}

    G_SysEventEmitter:on(SysEvent.ON_MOUSE_MOVE, function(event)
        if self.cache_MouseDownStatus[event:getMouseButton()] or self:isContainMouse() then
            self.eventDispatcher:dispatchEvent(event)
            -- print("move", _MyG.MouseEventDispatcher:getCursorX(), _MyG.MouseEventDispatcher:getCursorY())
        end
    end, self)

    G_SysEventEmitter:on(SysEvent.ON_MOUSE_SCROLL, function(event)
        if self:isContainMouse() then
            self.eventDispatcher:dispatchEvent(event)
            print("scroll", _MyG.MouseEventDispatcher:getCursorX(), _MyG.MouseEventDispatcher:getCursorY())
        end
    end, self)

    G_SysEventEmitter:on(SysEvent.ON_MOUSE_DOWN, function(event)
        if self:isContainMouse() then
            self.cache_MouseDownStatus[event:getMouseButton()] = true
            self.eventDispatcher:dispatchEvent(event)
            -- print("down", _MyG.MouseEventDispatcher:getCursorX(), _MyG.MouseEventDispatcher:getCursorY())
        end
    end, self)

    G_SysEventEmitter:on(SysEvent.ON_MOUSE_UP, function(event)
        local valid = self.cache_MouseDownStatus[event:getMouseButton()] or self:isContainMouse()
        self.cache_MouseDownStatus[event:getMouseButton()] = false
        if valid then
            self.eventDispatcher:dispatchEvent(event)
        end
    end, self)

    G_SysEventEmitter:on(SysEvent.ON_TOUCH_BEGAN, function(event)
        if self:isContainMouse() then
            self.eventDispatcher:dispatchEvent(event)
        end
    end, self)

    G_SysEventEmitter:on(SysEvent.ON_TOUCH_MOVED, function(event)
        self.eventDispatcher:dispatchEvent(event)
    end, self)


    G_SysEventEmitter:on(SysEvent.ON_TOUCH_ENDED, function(event)
        self.eventDispatcher:dispatchEvent(event)
    end, self)

    G_SysEventEmitter:on(SysEvent.ON_TOUCH_CANCELLED, function(event)
        self.eventDispatcher:dispatchEvent(event)
    end, self)

    self.eventDispatcher = cc.EventDispatcher:new()
    self.eventDispatcher:setEnabled(true)
    self.eventDispatcher:retain()
end

return Content