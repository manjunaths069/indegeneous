@echo off
set CLASSPATH=%CLASSPATH%;%BMC_PROACTIVENET_HOME%\apps3rdparty\jdbc\jconn3.jar;.
if /I "%1" == "" (
echo Please provide the device display name name as an argument. 
) else (
set /p ans=Do you want to delete the device %2 and associated instances? Please confirm [y/n]
IF /I "%ans%"== "y" (
	IF /I "%1" == "-devicename" (
		IF /I "%2" == "" (
			echo please provide the device display name value as an argument.
		) else (
			echo Start deleting device and associated instances.
			java DeleteDeviceAndAssociatedInstances -serverpath "%BMC_PROACTIVENET_HOME%" -devicename %2
			echo Execution completed.	
		)
	) else (
		IF /I "%1" == "-hostid" ( 
			
			IF /I "%2" == "" (
			       echo Please provide the host id value as an argument.
			) else (
			       java DeleteDeviceAndAssociatedInstances -serverpath "%BMC_PROACTIVENET_HOME%" -hostid %2
			       echo Execution completed.
			) 
			
		) else (
			echo Please provide the valid argument. -hostid hostid or -devicename device display name
		)
	)
	
) else (   
   if /I "%ans%"== "n" (
     echo You have entered %ans%. 
   ) else (
     echo You have entered: %ans%. Please enter the proper key "y or n"
   ) 
)
)