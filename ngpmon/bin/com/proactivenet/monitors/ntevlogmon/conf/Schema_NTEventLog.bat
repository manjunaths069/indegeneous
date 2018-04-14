REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'NTEVELOG_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Windows NT/2000 Event
REM Schema & PruneCntl Statements for Monitor = Windows NT/2000 Event ,moTypeId = 27351

echo Creating tables NTEVELOG_STATS, NTEVELOG_RT, NTEVELOG_BL
echo Creating Prune Control entries for NTEVELOG_STATS  AND  entries for NTEVELOG_RT
sqli "CREATE TABLE NTEVELOG_STATS (ITEMID int,TIMERECORDED int,EVENTS int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index NTEVELOG_STATS_N3 on NTEVELOG_STATS (TIMERECORDED);"

sqli "CREATE TABLE NTEVELOG_RT (ITEMID int, FROMTIME int, TOTIME int, EVENTS_HIGH int, EVENTS_AVG  int, EVENTS_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index NTEVELOG_RT_N3 on NTEVELOG_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE NTEVELOG_BL (ITEMID   int,TIMESLOT int,EVENTS_HIGH int,EVENTS_AVG  int,EVENTS_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NTEVELOG_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NTEVELOG_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Windows NT/2000 Event

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
