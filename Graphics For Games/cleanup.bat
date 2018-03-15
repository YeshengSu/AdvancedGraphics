
REM Remove Temp Visual Studio Files
REM *******************************

del	/s 	*.aps
del	/s 	*.clw
del	/s 	*.ncb
del	/s 	*.opt
del	/s 	*.plg
del	/s 	*.pdb
rem del	/s 	*.user
del	/s 	*.vsp
del	/s 	*.sdf
del	/s 	/A -H *.suo




rem del	/Q Debug\\*.*
rem rmdir 	/Q Debug
rem del   	/Q Release\\*.*
rem rmdir 	/Q Release

FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *ipch') DO RMDIR /S /Q "%%G"

rem FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *obj') DO RMDIR /S /Q "%%G"

rem FOR /F "tokens=*" %%G IN ('DIR /B /AD /S *bin') DO RMDIR /S /Q "%%G"
