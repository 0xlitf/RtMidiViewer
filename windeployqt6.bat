@echo off
rem set "destinationFolder=./"

rem xcopy "C:\NanGuangRepos\3rdParty\opencv\build\x64\vc16\bin\opencv_world480.dll" "%destinationFolder%\" /s /i /y

C:\Qt\6.6.0\msvc2019_64\bin\windeployqt6.exe app.exe
