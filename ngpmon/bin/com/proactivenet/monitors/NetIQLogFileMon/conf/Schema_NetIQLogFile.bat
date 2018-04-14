REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'NQ_LOGFILE_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adapter for NetIQ AppManager LogFile
REM Schema & PruneCntl Statements for Monitor = Adapter for NetIQ AppManager LogFile ,moTypeId = 44026

echo Creating tables NQ_LOGFILE_STATS, NQ_LOGFILE_RT, NQ_LOGFILE_BL
echo Creating Prune Control entries for NQ_LOGFILE_STATS  AND  entries for NQ_LOGFILE_RT
sqli "CREATE TABLE NQ_LOGFILE_STATS (ITEMID int,TIMERECORDED int,DATA_AVAIL int,OCCURANCES int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index NQ_LOGFILE_STATS_N3 on NQ_LOGFILE_STATS (TIMERECORDED);"

sqli "CREATE TABLE NQ_LOGFILE_RT (ITEMID int, FROMTIME int, TOTIME int, DATA_AVAIL_HIGH int, DATA_AVAIL_AVG  int, DATA_AVAIL_LOW  int, OCCURANCES_HIGH int, OCCURANCES_AVG  int, OCCURANCES_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index NQ_LOGFILE_RT_N3 on NQ_LOGFILE_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE NQ_LOGFILE_BL (ITEMID   int,TIMESLOT int,STARTTIME int,DURATION int,DATA_AVAIL_HIGH int,DATA_AVAIL_AVG  int,DATA_AVAIL_LOW  int,OCCURANCES_HIGH int,OCCURANCES_AVG  int,OCCURANCES_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "STARTTIME", "DURATION") WITH HASH SIZE 10);"

sqli "create index NQ_LOGFILE_BL_N2 on NQ_LOGFILE_BL ( STARTTIME, DURATION );"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_LOGFILE_STATS', 'TIMERECORDED', 192);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_LOGFILE_RT', 'FROMTIME', 2160);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_LOGFILE_BL', 'STARTTIME', 8760);"

) else (

REM Upgrade Installation for Monitor = Adapter for NetIQ AppManager LogFile

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
