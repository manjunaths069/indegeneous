REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'ORAROLBAK_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Oracle Rollback Segment
REM Schema & PruneCntl Statements for Monitor = Oracle Rollback Segment ,moTypeId = 41010

echo Creating tables ORAROLBAK_STATS, ORAROLBAK_RT, ORAROLBAK_BL
echo Creating Prune Control entries for ORAROLBAK_STATS  AND  entries for ORAROLBAK_RT
sqli "CREATE TABLE ORAROLBAK_STATS (ITEMID int,TIMERECORDED int,EXTENTS int,WRITES int,GETS int,WAITS int,SHRINKS int,WRAPS int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index ORAROLBAK_STATS_N3 on ORAROLBAK_STATS (TIMERECORDED);"

sqli "CREATE TABLE ORAROLBAK_RT (ITEMID int, FROMTIME int, TOTIME int, EXTENTS_HIGH int, EXTENTS_AVG  int, EXTENTS_LOW  int, WRITES_HIGH int, WRITES_AVG  int, WRITES_LOW  int, GETS_HIGH int, GETS_AVG  int, GETS_LOW  int, WAITS_HIGH int, WAITS_AVG  int, WAITS_LOW  int, SHRINKS_HIGH int, SHRINKS_AVG  int, SHRINKS_LOW  int, WRAPS_HIGH int, WRAPS_AVG  int, WRAPS_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index ORAROLBAK_RT_N3 on ORAROLBAK_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE ORAROLBAK_BL (ITEMID   int,TIMESLOT int,EXTENTS_HIGH int,EXTENTS_AVG  int,EXTENTS_LOW  int,WRITES_HIGH int,WRITES_AVG  int,WRITES_LOW  int,GETS_HIGH int,GETS_AVG  int,GETS_LOW  int,WAITS_HIGH int,WAITS_AVG  int,WAITS_LOW  int,SHRINKS_HIGH int,SHRINKS_AVG  int,SHRINKS_LOW  int,WRAPS_HIGH int,WRAPS_AVG  int,WRAPS_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('ORAROLBAK_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('ORAROLBAK_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Oracle Rollback Segment

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
