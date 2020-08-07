cls

@echo off

echo.
echo. 
echo.
echo. 
echo      \\\\\\\          \\\\\\       \\          \\     \\\\\\\\      \\\\\\    \\           \\\\\\\\ 
echo    \\       \\      \\      \\     \\\\      \\\\     \\      \\      \\      \\           \\ 
echo    \\              \\        \\    \\  \\  \\  \\     \\       \\     \\      \\           \\   
echo    \\              \\        \\    \\    \\    \\     \\      \\      \\      \\           \\\\\ 
echo    \\              \\        \\    \\          \\     \\\\\\\\        \\      \\           \\ 
echo    \\        \\     \\      \\     \\          \\     \\              \\      \\           \\ 
echo      \\\\\\\\         \\\\\\       \\          \\     \\            \\\\\\    \\\\\\\\\\   \\\\\\\\\
echo.
echo. 
echo.
echo.

@echo on

emcc -o index.html c-code/main.c c-code/esUtil.c -s USE_SDL=2
REM emcc -o index.html c-code/Hello_Triangle.c c-code/esUtil.c -s USE_SDL=2
REM-msimd128 

REM -s ASSERTIONS=1
