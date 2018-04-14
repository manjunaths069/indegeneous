REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'RADIUSRESP_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = RADIUS
REM Schema & PruneCntl Statements for Monitor = RADIUS ,moTypeId = 20081

echo Creating tables RADIUSRESP_STATS, RADIUSRESP_RT, RADIUSRESP_BL
echo Creating Prune Control entries for RADIUSRESP_STATS  AND  entries for RADIUSRESP_RT
sqli "CREATE TABLE RADIUSRESP_STATS (ITEMID int,TIMERECORDED int,RESPTIME int,AVAILABILITY int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index RADIUSRESP_STATS_N3 on RADIUSRESP_STATS (TIMERECORDED);"

sqli "CREATE TABLE RADIUSRESP_RT (ITEMID int, FROMTIME int, TOTIME int, RESPTIME_HIGH int, RESPTIME_AVG  int, RESPTIME_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index RADIUSRESP_RT_N3 on RADIUSRESP_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE RADIUSRESP_BL (ITEMID   int,TIMESLOT int,RESPTIME_HIGH int,RESPTIME_AVG  int,RESPTIME_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('RADIUSRESP_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('RADIUSRESP_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = RADIUS

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
