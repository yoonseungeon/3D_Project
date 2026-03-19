:: 명령어			옵션		원본 파일의 위치			사본 파일을 저장할 위치

xcopy			/y		.\Engine\Public\*.h		.\EngineSDK\Inc\
xcopy			/y		.\Engine\Bin\*.dll			.\Client\Bin\
xcopy			/y		.\Engine\Bin\Engine.lib		.\EngineSDK\lib\

:: FMOD DLL 복사
if /i "%1"=="Debug" (
    xcopy /y ".\External\FMOD\lib\x64\fmodL.dll" ".\Client\Bin\"
) else (
    xcopy /y ".\External\FMOD\lib\x64\fmod.dll"  ".\Client\Bin\"
)