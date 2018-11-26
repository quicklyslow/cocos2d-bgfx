setlocal
cd .\Shader
call shaderbin.bat
call shaderbindx9.bat
call shaderbindx11.bat
call shaderbinessl.bat
call shaderbinmetal.bat
endlocal
xcopy ".\Shader\shader" ".\simulator\win32\shader" /s /e /y /i
xcopy ".\cocos\scripting\js-bindings\script" ".\simulator\win32\script" /s /e /y /i