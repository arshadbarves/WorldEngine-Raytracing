@echo off

pushd ..
vendor\bin\premake\Windows\premake5.exe --file=Build.lua vs2022
popd
pause