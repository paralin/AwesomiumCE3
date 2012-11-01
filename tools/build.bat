@echo off

:: Set project relevant settings
set VCPROJECT="..\project\Awesomium.vcxproj"
set VCTOOLS="%VS110COMNTOOLS%..\..\VC\vcvarsall.bat"

IF EXIST %VCTOOLS% (
  :: Compile x86
  call %VCTOOLS% x86

  MSBuild %VCPROJECT% /t:Rebuild /p:Configuration=Release
  IF ERRORLEVEL 1 GOTO COMPILERROR


  :: Compile x64
  :: Awesomium not x64 compatible yet
)

:: End
GOTO ENDOK

:COMPILERROR

::Trigger a Syntax error
--ERROR_DETECTED--

:ENDOK
