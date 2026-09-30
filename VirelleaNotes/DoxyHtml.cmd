@echo off
chcp 65001 >nul

REM =====================================================
REM Doxygen ドキュメント生成バッチ
REM =====================================================

REM 作業ディレクトリ移動
cd /d "%~dp0Document\Doxygen"
if errorlevel 1 (
    echo フォルダ移動に失敗しました
    pause
    exit /b
)

REM doxygen存在チェック
where doxygen >nul 2>&1
if errorlevel 1 (
    echo doxygen が見つかりません（PATH未設定）
    pause
    exit /b
)

REM ドキュメント生成
doxygen Doxyfile
if errorlevel 1 (
    echo ドキュメント生成に失敗しました
    pause
    exit /b
)

echo ドキュメント生成完了

REM HTMLパス
set "HTML_PATH=%CD%\html\index.html"

REM ファイル確認して開く
if exist "%HTML_PATH%" (
    start "" "%HTML_PATH%"
) else (
    echo index.html が見つかりません: %HTML_PATH%
    pause
)