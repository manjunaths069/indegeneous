REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'TMWProcess_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adapter for Tivoli Windows Process
REM Schema & PruneCntl Statements for Monitor = Adapter for Tivoli Windows Process ,moTypeId = 31253

echo Creating tables TMWProcess_STATS, TMWProcess_RT, TMWProcess_BL
echo Creating Prune Control entries for TMWProcess_STATS  AND  entries for TMWProcess_RT
sqli "CREATE TABLE TMWProcess_STATS (ITEMID int,TIMERECORDED int,CPUUSAGE int,HNDCOUNT int,PROCAVAIL int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index TMWProcess_STATS_N3 on TMWProcess_STATS (TIMERECORDED);"

sqli "CREATE TABLE TMWProcess_RT (ITEMID int, FROMTIME int, TOTIME int, CPUUSAGE_HIGH int, CPUUSAGE_AVG  int, CPUUSAGE_LOW  int, HNDCOUNT_HIGH int, HNDCOUNT_AVG  int, HNDCOUNT_LOW  int, PROCAVAIL_HIGH int, PROCAVAIL_AVG  int, PROCAVAIL_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index TMWProcess_RT_N3 on TMWProcess_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE TMWProcess_BL (ITEMID   int,TIMESLOT int,STARTTIME int,DURATION int,CPUUSAGE_HIGH int,CPUUSAGE_AVG  int,CPUUSAGE_LOW  int,HNDCOUNT_HIGH int,HNDCOUNT_AVG  int,HNDCOUNT_LOW  int,PROCAVAIL_HIGH int,PROCAVAIL_AVG  int,PROCAVAIL_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "STARTTIME", "DURATION") WITH HASH SIZE 10);"

sqli "create index TMWProcess_BL_N2 on TMWProcess_BL ( STARTTIME, DURATION );"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TMWProcess_STATS', 'TIMERECORDED', 192);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TMWProcess_RT', 'FROMTIME', 2160);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TMWProcess_BL', 'STARTTIME', 8760);"

) else (

REM Upgrade Installation for Monitor = Adapter for Tivoli Windows Process

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
