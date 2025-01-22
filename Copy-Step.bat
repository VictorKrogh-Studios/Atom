@ECHO OFF

ECHO Copying file to: %1

XCOPY .\Atom\Include .\bin\%1\Atom\Include\ /S

XCOPY .\Sandbox\Assets\ .\bin\%1\Sandbox\Assets\ /S
