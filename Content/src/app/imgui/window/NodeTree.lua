
local NodeTree = class("NodeTree", require("app.imgui.Window"))

function NodeTree:ctor()
    NodeTree.super.ctor(self, self.__cname, true)
end

function NodeTree:onGUI()
    if self.img_ui_io == nil then
        self.img_ui_io = ImGui.GetIO()
    end

    ImGui.Begin(self.winName)
    G_SysEventEmitter:emit(SysEvent.ON_NODE_CONTENT_GUI, self)
    ImGui.End()

    G_SysEventEmitter:emit(SysEvent.ON_NODE_CONTENT_GUI_END)

    if ImGui.BeginPopup("node_content_right_click_popup") and self.popLogic then
        self.popLogic()
        ImGui.EndPopup()
    end
end

function NodeTree:setPopLogic(func)
    self.popLogic = func
end

function NodeTree:cosutomDrawItem(item, noDrag)
    local height = ImGui.GetFrameHeight()
    local iconSize = {x = height, y = height}

    if item.noOpen then
        ImGui.Image(self.fileIcon, iconSize)
    else
        ImGui.Image(self.folderIcon, iconSize)
    end
    ImGui.SameLine()

    if noDrag then
        ImGui.Selectable(item.name, false, ImGuiSelectableFlags_AllowDoubleClick)
        return
    end

    if ImGui.Selectable(item.name, false, ImGuiSelectableFlags_AllowDoubleClick) then
        if ImGui.IsMouseDoubleClicked(0) then
            if not item.noOpen then
                item.__isOpen = not item.__isOpen
            else
                if self.leftDoubleClickCall then
                    self.leftDoubleClickCall(item)
                end
            end
        end
    end
    
    if ImGui.IsItemHovered() then
        if self.onItemHoveredCallback then
            self.onItemHoveredCallback(item)
        end
        -- 右键点击
        if ImGui.IsMouseClicked(1) then
            if self.rightClickCall then
                self.rightClickCall(item)
            end
        end
    end
end

function NodeTree:drawItem(item)

    if item == nil then
        return
    end

    if item.__nodeKey == nil then
        item.__nodeKey = tostring(self.uniqueID)
        self.uniqueID = self.uniqueID + 1
        item.__buttonKey = tostring(self.uniqueID)
        self.uniqueID = self.uniqueID + 1
    end

    if not item.noOpen then
        if item.__isOpen then
            if ImGui.ArrowButton(item.__buttonKey, ImGuiDir_Down) then
                item.__isOpen = not item.__isOpen
            end
        else
            if ImGui.ArrowButton(item.__buttonKey, ImGuiDir_Right) then
                item.__isOpen = not item.__isOpen
            end
        end
        ImGui.SameLine()
        self:cosutomDrawItem(item)

        if item.__isOpen then
            if item.dataDirty then
                item.dataDirty = false
                print("doseek", item.name)
                self:doseek(item)
            end
            if item.items then
                local frameHeight = ImGui.GetFrameHeight()
                ImGui.Indent(frameHeight)
                for k, v in pairs(item.items) do
                    self:drawItem(v)
                end
                ImGui.Unindent(frameHeight)
            end
        end
    else
        self:cosutomDrawItem(item)
    end
end

return NodeTree