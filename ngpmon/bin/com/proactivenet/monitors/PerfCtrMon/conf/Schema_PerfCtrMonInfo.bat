REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'NTPERFCTR_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Windows NT/2000 Performance Counter
REM Schema & PruneCntl Statements for Monitor = Windows NT/2000 Performance Counter ,moTypeId = 27401

echo Creating tables NTPERFCTR_STATS, NTPERFCTR_RT, NTPERFCTR_BL
echo Creating Prune Control entries for NTPERFCTR_STATS  AND  entries for NTPERFCTR_RT
sqli "CREATE TABLE NTPERFCTR_STATS (ITEMID int,TIMERECORDED int,CTRVALUE int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index NTPERFCTR_STATS_N3 on NTPERFCTR_STATS (TIMERECORDED);"

sqli "CREATE TABLE NTPERFCTR_RT (ITEMID int, FROMTIME int, TOTIME int, CTRVALUE_HIGH int, CTRVALUE_AVG  int, CTRVALUE_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index NTPERFCTR_RT_N3 on NTPERFCTR_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE NTPERFCTR_BL (ITEMID   int,TIMESLOT int,CTRVALUE_HIGH int,CTRVALUE_AVG  int,CTRVALUE_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NTPERFCTR_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NTPERFCTR_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Windows NT/2000 Performance Counter

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
