@echo off
chcp 65001
setlocal enabledelayedexpansion

set TARGET_DIR=%~dp0/src/
set TOTAL_LINES=0

echo ファイルごとの行数:
echo ------------------------

:: --- .cpp ファイル ---
for /R "%TARGET_DIR%" %%F in (*.cpp) do (
    for /f %%L in ('find /v /c "" ^< "%%F"') do (
        set FILE_LINES=%%L
        echo %%F : !FILE_LINES! 行
        set /a TOTAL_LINES+=!FILE_LINES!
    )
)

:: --- .h ファイル ---
for /R "%TARGET_DIR%" %%F in (*.h) do (
    for /f %%L in ('find /v /c "" ^< "%%F"') do (
        set FILE_LINES=%%L
        echo %%F : !FILE_LINES! 行
        set /a TOTAL_LINES+=!FILE_LINES!
    )
)

echo ------------------------
echo 合計行数: !TOTAL_LINES! 行
pause
