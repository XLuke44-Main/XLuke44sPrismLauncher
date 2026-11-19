@echo off
setlocal EnableExtensions DisableDelayedExpansion
call "%~dp0clean-windows-portable.bat"
call "%~dp0build-windows-portable.bat"