@echo off
set JAR_FILE_NAME=lipiDesigner.jar
set CURRENT_WORKING_DIR=%cd%

if [%LIPI_ROOT%] == [] (
echo "Environment variable LIPI_ROOT not set. Exiting..."
goto exit
)

cd %LIPI_ROOT%/lipiDesigner/src/javauserinterface/
javac *.java
jar cfm %JAR_FILE_NAME% Manifest.txt *.class images
del *.class
move/y %JAR_FILE_NAME% "%LIPI_ROOT%/lipiDesigner/bin"
cd %CURRENT_WORKING_DIR%

echo "The output Jar is available under %LIPI_ROOT%/lipiDesigner/bin"

:exit
set JAR_FILE_NAME=
set CURRENT_WORKING_DIR=