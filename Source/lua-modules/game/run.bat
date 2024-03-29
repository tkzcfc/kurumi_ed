@echo off
set curDir=%~d0%~p0
tolua++ "-L" "%curDir%../basic.lua" "-o" "lua_game.cpp" "game.pkg"
pause