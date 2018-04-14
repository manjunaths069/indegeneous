REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'IISASP_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = IIS Active Server Pages
REM Schema & PruneCntl Statements for Monitor = IIS Active Server Pages ,moTypeId = 27301

echo Creating tables IISASP_STATS, IISASP_RT, IISASP_BL
echo Creating Prune Control entries for IISASP_STATS  AND  entries for IISASP_RT
sqli "CREATE TABLE IISASP_STATS (ITEMID int,TIMERECORDED int,ERRSPERSEC int,MEMALLOC int,REQEXECTIME int,REQEXEING int,REQQUEUED int,REQREJECTED int,REQTIMEDOUT int,SESSCURR int,TRNSEC int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index IISASP_STATS_N3 on IISASP_STATS (TIMERECORDED);"

sqli "CREATE TABLE IISASP_RT (ITEMID int, FROMTIME int, TOTIME int, ERRSPERSEC_HIGH int, ERRSPERSEC_AVG  int, ERRSPERSEC_LOW  int, MEMALLOC_HIGH int, MEMALLOC_AVG  int, MEMALLOC_LOW  int, REQEXECTIME_HIGH int, REQEXECTIME_AVG  int, REQEXECTIME_LOW  int, REQEXEING_HIGH int, REQEXEING_AVG  int, REQEXEING_LOW  int, REQQUEUED_HIGH int, REQQUEUED_AVG  int, REQQUEUED_LOW  int, REQREJECTED_HIGH int, REQREJECTED_AVG  int, REQREJECTED_LOW  int, REQTIMEDOUT_HIGH int, REQTIMEDOUT_AVG  int, REQTIMEDOUT_LOW  int, SESSCURR_HIGH int, SESSCURR_AVG  int, SESSCURR_LOW  int, TRNSEC_HIGH int, TRNSEC_AVG  int, TRNSEC_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index IISASP_RT_N3 on IISASP_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE IISASP_BL (ITEMID   int,TIMESLOT int,ERRSPERSEC_HIGH int,ERRSPERSEC_AVG  int,ERRSPERSEC_LOW  int,MEMALLOC_HIGH int,MEMALLOC_AVG  int,MEMALLOC_LOW  int,REQEXECTIME_HIGH int,REQEXECTIME_AVG  int,REQEXECTIME_LOW  int,REQEXEING_HIGH int,REQEXEING_AVG  int,REQEXEING_LOW  int,REQQUEUED_HIGH int,REQQUEUED_AVG  int,REQQUEUED_LOW  int,REQREJECTED_HIGH int,REQREJECTED_AVG  int,REQREJECTED_LOW  int,REQTIMEDOUT_HIGH int,REQTIMEDOUT_AVG  int,REQTIMEDOUT_LOW  int,SESSCURR_HIGH int,SESSCURR_AVG  int,SESSCURR_LOW  int,TRNSEC_HIGH int,TRNSEC_AVG  int,TRNSEC_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('IISASP_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('IISASP_RT', 'FROMTIME', 2160);"
sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('IISASP_BL', 'STARTTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = IIS Active Server Pages

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
