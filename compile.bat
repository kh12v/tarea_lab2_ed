@echo off
echo Compiling main.cpp and pugixml.cpp...
g++ main.cpp pugixml/pugixml.cpp tree.cpp -o main.exe -std=c++17

if %errorlevel% neq 0 (
    echo.
    echo Compilation failed.
) else (
    echo.
    echo Compilation successful! 
    echo Running main.exe...
    echo ----------------------------------------
    main.exe
    echo ----------------------------------------
)
pause
