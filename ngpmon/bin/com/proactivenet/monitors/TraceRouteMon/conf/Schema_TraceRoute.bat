REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'TRACE_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = TraceRoute
REM Schema & PruneCntl Statements for Monitor = TraceRoute ,moTypeId = 40001

echo Creating tables TRACE_STATS, TRACE_RT, TRACE_BL
echo Creating Prune Control entries for TRACE_STATS  AND  entries for TRACE_RT
sqli "CREATE TABLE TRACE_STATS (ITEMID int,TIMERECORDED int,AVAIL int,REACHT int,NOOFHOP int,MAXHOPT int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index TRACE_STATS_N3 on TRACE_STATS (TIMERECORDED);"

sqli "CREATE TABLE TRACE_RT (ITEMID int, FROMTIME int, TOTIME int, AVAIL_HIGH int, AVAIL_AVG  int, AVAIL_LOW  int, REACHT_HIGH int, REACHT_AVG  int, REACHT_LOW  int, NOOFHOP_HIGH int, NOOFHOP_AVG  int, NOOFHOP_LOW  int, MAXHOPT_HIGH int, MAXHOPT_AVG  int, MAXHOPT_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index TRACE_RT_N3 on TRACE_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE TRACE_BL (ITEMID   int,TIMESLOT int,AVAIL_HIGH int,AVAIL_AVG  int,AVAIL_LOW  int,REACHT_HIGH int,REACHT_AVG  int,REACHT_LOW  int,NOOFHOP_HIGH int,NOOFHOP_AVG  int,NOOFHOP_LOW  int,MAXHOPT_HIGH int,MAXHOPT_AVG  int,MAXHOPT_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TRACE_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TRACE_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = TraceRoute

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
