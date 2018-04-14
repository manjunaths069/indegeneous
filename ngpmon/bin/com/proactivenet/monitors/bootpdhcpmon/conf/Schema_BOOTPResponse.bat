REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'BOOTP_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = BOOTP
REM Schema & PruneCntl Statements for Monitor = BOOTP ,moTypeId = 20101

echo Creating tables BOOTP_STATS, BOOTP_RT, BOOTP_BL
echo Creating Prune Control entries for BOOTP_STATS  AND  entries for BOOTP_RT
sqli "CREATE TABLE BOOTP_STATS (ITEMID int,TIMERECORDED int,RESPTIME int,AVAILABILITY int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index BOOTP_STATS_N3 on BOOTP_STATS (TIMERECORDED);"

sqli "CREATE TABLE BOOTP_RT (ITEMID int, FROMTIME int, TOTIME int, RESPTIME_HIGH int, RESPTIME_AVG  int, RESPTIME_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index BOOTP_RT_N3 on BOOTP_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE BOOTP_BL (ITEMID   int,TIMESLOT int,RESPTIME_HIGH int,RESPTIME_AVG  int,RESPTIME_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BOOTP_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BOOTP_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = BOOTP

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
