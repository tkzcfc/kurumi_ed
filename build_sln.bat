@echo off

set AX_ROOT=D:\fff\test\git_axis
set AX_CONSOLE_ROOT=D:\999_client\axis\tools\console\bin

%AX_ROOT%\tools\external\cmake\bin\cmake -S . -B build -G "Visual Studio 17 2022" -A x64
pause