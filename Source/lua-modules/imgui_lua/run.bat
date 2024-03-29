@echo off
set curDir=%~d0%~p0
tolua++ "-L" "%curDir%../basic.lua" "-o" "imgui_tolua.cpp" "main.pkg"
pause