REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'NODE_RESP_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Old Ping
REM Schema & PruneCntl Statements for Monitor = Old Ping ,moTypeId = 23000

echo Creating tables NODE_RESP_STATS, NODE_RESP_RT, NODE_RESP_BL
echo Creating Prune Control entries for NODE_RESP_STATS  AND  entries for NODE_RESP_RT
sqli "CREATE TABLE NODE_RESP_STATS (ITEMID int,TIMERECORDED int,RESPTIME int,AVAILABILITY int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index NODE_RESP_STATS_N3 on NODE_RESP_STATS (TIMERECORDED);"

sqli "CREATE TABLE NODE_RESP_RT (ITEMID int, FROMTIME int, TOTIME int, RESPTIME_HIGH int, RESPTIME_AVG  int, RESPTIME_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index NODE_RESP_RT_N3 on NODE_RESP_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE NODE_RESP_BL (ITEMID   int,TIMESLOT int,RESPTIME_HIGH int,RESPTIME_AVG  int,RESPTIME_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NODE_RESP_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NODE_RESP_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Old Ping

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
