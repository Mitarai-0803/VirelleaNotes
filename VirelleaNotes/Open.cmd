@echo off
@setlocal enabledelayedexpansion

REM =====================================
REM 文字コードを UTF-8 に設定
REM =====================================
chcp 65001 > nul

REM =====================================
REM 現在のバッチファイルの場所に移動
REM =====================================
pushd "%~dp0"

REM =====================================
REM プロジェクト生成 (Make.cmd など)
REM =====================================
call "Make.cmd"
call "@code_format.cmd" User

REM =====================================
REM .build フォルダ内の .sln を検索
REM =====================================
set SOLUTION_FILE=

REM .build フォルダに移動して検索
cd ".build"
for /f "usebackq delims=" %%i in (`dir /b *.sln`) do (
    set SOLUTION_FILE=%%i
)

REM 見つからなければエラー
if not defined SOLUTION_FILE (
    echo [ERROR] .sln ファイルが .build に見つかりません。
    echo Make.cmd を実行してください。
    goto error
)

REM =====================================
REM 最新の Visual Studio を検索
REM =====================================
REM VS Installer パスを追加
set PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\Installer

REM 最新版の Visual Studio の devenv.exe を取得
for /f "usebackq delims=" %%i in (`vswhere.exe -latest -property productPath`) do (
    set DEVENV_EXE=%%i
)

REM 見つからなければエラー
if not defined DEVENV_EXE (
    echo [ERROR] Visual Studio が見つかりません。
    goto error
)

REM =====================================
REM ソリューションを最小化で起動
REM =====================================
start /min "" "%DEVENV_EXE%" "%SOLUTION_FILE%"

REM =====================================
REM 元のディレクトリに戻る
REM =====================================
popd
exit /b 0

:error
REM エラー時もディレクトリを戻す
popd
pause
exit /b 1