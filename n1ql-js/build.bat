if not exist "build" mkdir  "build"
cd build
REM if exist "CMakeCache.txt" del CMakeCache.txt
cmake -G "Visual Studio 15 2017 Win64" ..
msbuild.exe n1ql_js.sln
cd Debug
copy ..\..\deps\v8\bin\* .
copy ..\..\deps\v8\libs\* .