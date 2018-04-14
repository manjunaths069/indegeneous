REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'SYSTMWAIT_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Oracle System Wait
REM Schema & PruneCntl Statements for Monitor = Oracle System Wait ,moTypeId = 41026

echo Creating tables SYSTMWAIT_STATS, SYSTMWAIT_RT, SYSTMWAIT_BL
echo Creating Prune Control entries for SYSTMWAIT_STATS  AND  entries for SYSTMWAIT_RT
sqli "CREATE TABLE SYSTMWAIT_STATS (ITEMID int,TIMERECORDED int,Time_Waited int,P_Time_Waited int,Waits int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index SYSTMWAIT_STATS_N3 on SYSTMWAIT_STATS (TIMERECORDED);"

sqli "CREATE TABLE SYSTMWAIT_RT (ITEMID int, FROMTIME int, TOTIME int, Time_Waited_HIGH int, Time_Waited_AVG  int, Time_Waited_LOW  int, P_Time_Waited_HIGH int, P_Time_Waited_AVG  int, P_Time_Waited_LOW  int, Waits_HIGH int, Waits_AVG  int, Waits_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index SYSTMWAIT_RT_N3 on SYSTMWAIT_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE SYSTMWAIT_BL (ITEMID   int,TIMESLOT int,Time_Waited_HIGH int,Time_Waited_AVG  int,Time_Waited_LOW  int,P_Time_Waited_HIGH int,P_Time_Waited_AVG  int,P_Time_Waited_LOW  int,Waits_HIGH int,Waits_AVG  int,Waits_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('SYSTMWAIT_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('SYSTMWAIT_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Oracle System Wait

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
