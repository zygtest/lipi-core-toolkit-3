@echo off
cd %LIPI_ROOT%\scripts
packageRecognizer.pl -project %1 -logicalname %2
EXIT /B