REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'TMWSystem_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adapter for Tivoli Windows System
REM Schema & PruneCntl Statements for Monitor = Adapter for Tivoli Windows System ,moTypeId = 31251

echo Creating tables TMWSystem_STATS, TMWSystem_RT, TMWSystem_BL
echo Creating Prune Control entries for TMWSystem_STATS  AND  entries for TMWSystem_RT
sqli "CREATE TABLE TMWSystem_STATS (ITEMID int,TIMERECORDED int,INTERRPTS int,PROCNUM int,REALMEMAVAIL int,SYSCPU int,TOTCPU int,USRCPU int,SYSTAVAIL int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index TMWSystem_STATS_N3 on TMWSystem_STATS (TIMERECORDED);"

sqli "CREATE TABLE TMWSystem_RT (ITEMID int, FROMTIME int, TOTIME int, INTERRPTS_HIGH int, INTERRPTS_AVG  int, INTERRPTS_LOW  int, PROCNUM_HIGH int, PROCNUM_AVG  int, PROCNUM_LOW  int, REALMEMAVAIL_HIGH int, REALMEMAVAIL_AVG  int, REALMEMAVAIL_LOW  int, SYSCPU_HIGH int, SYSCPU_AVG  int, SYSCPU_LOW  int, TOTCPU_HIGH int, TOTCPU_AVG  int, TOTCPU_LOW  int, USRCPU_HIGH int, USRCPU_AVG  int, USRCPU_LOW  int, SYSTAVAIL_HIGH int, SYSTAVAIL_AVG  int, SYSTAVAIL_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index TMWSystem_RT_N3 on TMWSystem_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE TMWSystem_BL (ITEMID   int,TIMESLOT int,STARTTIME int,DURATION int,INTERRPTS_HIGH int,INTERRPTS_AVG  int,INTERRPTS_LOW  int,PROCNUM_HIGH int,PROCNUM_AVG  int,PROCNUM_LOW  int,REALMEMAVAIL_HIGH int,REALMEMAVAIL_AVG  int,REALMEMAVAIL_LOW  int,SYSCPU_HIGH int,SYSCPU_AVG  int,SYSCPU_LOW  int,TOTCPU_HIGH int,TOTCPU_AVG  int,TOTCPU_LOW  int,USRCPU_HIGH int,USRCPU_AVG  int,USRCPU_LOW  int,SYSTAVAIL_HIGH int,SYSTAVAIL_AVG  int,SYSTAVAIL_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "STARTTIME", "DURATION") WITH HASH SIZE 10);"

sqli "create index TMWSystem_BL_N2 on TMWSystem_BL ( STARTTIME, DURATION );"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TMWSystem_STATS', 'TIMERECORDED', 192);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TMWSystem_RT', 'FROMTIME', 2160);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TMWSystem_BL', 'STARTTIME', 8760);"

) else (

REM Upgrade Installation for Monitor = Adapter for Tivoli Windows System

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
