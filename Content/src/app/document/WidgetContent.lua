local Document = require("app.imgui.Document")

local WidgetContent = class("WidgetContent", Document)


local const_item_size = {x = 80, y = 100}
local const_icon_size = {x = 50, y = 50}

local item_size = {x = 0, y = 0}
local icon_size = {x = 0, y = 0}

local color = Tools:getStyleColor(ImGuiCol_FrameBgHovered)
local FrameBgHovered_Color = IM_COL32(color.x * 255, color.y * 255, color.z * 255, color.w * 255)

color = Tools:getStyleColor(ImGuiCol_FrameBgActive)
local FrameBgActive_Color = IM_COL32(color.x * 255, color.y * 255, color.z * 255, color.w * 255)



function WidgetContent:ctor()
	WidgetContent.super.ctor(self, "widgets")

	self.scrolling_size = cc.p(0, 0)
	self.scrolling_flag = ImGuiWindowFlags_HorizontalScrollbar
    self.scrolling_flag = Tools:bor_int32(self.scrolling_flag, ImGuiWindowFlags_NoScrollbar)

	self.widgetDataArr = {}

	self:appendWidgetInfo(EditorIconContent:get(EditorIcon.DEFAULT_SPRITE), "WC_Sprite")
	self:appendWidgetInfo(EditorIconContent:get(EditorIcon.DEFAULT_IMAGE), "WC_Image")
	-- 复活点
	self:appendWidgetInfo(EditorIconContent:get(EditorIcon.DEFAULT_REVIVEPOINT), "WC_RevivePoint")
end

function WidgetContent:appendWidgetInfo(_icon, _text)
	table.insert(self.widgetDataArr, {icon = _icon, text = _text})
end

function WidgetContent:onGUI()
	self.cache_drag_item_last = self.cache_drag_item
	self.cache_drag_item = nil


	local fontScale = ImGui.GetIO().FontGlobalScale
	icon_size.x = const_icon_size.x * fontScale
	icon_size.y = const_icon_size.y * fontScale
	item_size.x = const_item_size.x * fontScale
	item_size.y = const_item_size.y * fontScale

	self:render()

	if self.cache_drag_item == nil then
		if self.cache_drag_item_last then
			G_SysEventEmitter:emit(SysEvent.ON_DRAG_PRE_WIDGET, self.cache_drag_item_last.text)
		end
	end
end

function WidgetContent:render()

	local count = self:getWidgetCount()
	local itemSpacing = ImGui.GetStyle().ItemSpacing.x
	local window_visible_x2 = ImGui.GetWindowPos().x + ImGui.GetWindowContentRegionMax().x

	for n = 1, count do
		ImGui.PushID(n)

		ImGui.BeginChild(n, item_size, true, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar)

		if ImGui.BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID) then
        	ImGui.SetDragDropPayload("DRAG_MOVE_WIDGET", self.widgetDataArr[n].text, #self.widgetDataArr[n].text)
        	self:drawDragWidget(self.widgetDataArr[n])
        	self.cache_drag_item = self.widgetDataArr[n]
        	ImGui.EndDragDropSource()
    	end

		ImGui.Image(self.widgetDataArr[n].icon, icon_size)
		ImGui.PushTextWrapPos(ImGui.GetCursorPos().x + item_size.x)
		ImGui.Text(STR(self.widgetDataArr[n].text))
		ImGui.PopTextWrapPos()

		ImGui.EndChild()

		-- self.widgetDataArr[n].isDrawBGRectTag = ImGui.IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)

        local last_button_x2 = ImGui.GetItemRectMax().x
        local next_button_x2 = last_button_x2 + itemSpacing+ item_size.x
		if n < count and next_button_x2 < window_visible_x2 then
			ImGui.SameLine()
		end

		ImGui.PopID()
	end
end

function WidgetContent:getWidgetCount()
	return #self.widgetDataArr
end

function WidgetContent:drawDragWidget(widgetData)
	ImGui.Image(widgetData.icon, icon_size)
	ImGui.PushTextWrapPos(ImGui.GetCursorPos().x + item_size.x)
	ImGui.Text(STR(widgetData.text))
	ImGui.PopTextWrapPos()
end

function WidgetContent:canRemove()
	return false
end

function WidgetContent:canClose()
	return false
end

return WidgetContent