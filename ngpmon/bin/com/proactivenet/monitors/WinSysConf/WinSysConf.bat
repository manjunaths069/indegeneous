@ECHO OFF

ipconfig /all
netstat -r
netstat -p tcp
%1\pronto\bin\GetOsPatches.exe
start /wait msinfo32.exe /report %temp%\WinSysConf.out /categories +SystemSummary+ComponentsProblemDevices+ResourcesConflicts+ResourcesForcedHardware
type %temp%\WinSysConf.out
del %temp%\WinSysConf.out

