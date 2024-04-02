



imgui.cpp 修改

```
    const bool render_dockspace_bg = node->IsRootNode() && host_window && (node_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0;
    if (render_dockspace_bg && node->IsVisible)
    
    修改为：
    
    if (false && render_dockspace_bg && node->IsVisible)
```

