REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'TRAPRECEIVER_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = SNMP Trap Receiver
REM Schema & PruneCntl Statements for Monitor = SNMP Trap Receiver ,moTypeId = 49001

echo Creating tables TRAPRECEIVER_STATS, TRAPRECEIVER_RT, TRAPRECEIVER_BL
echo Creating Prune Control entries for TRAPRECEIVER_STATS  AND  entries for TRAPRECEIVER_RT
sqli "CREATE TABLE TRAPRECEIVER_STATS (ITEMID int,TIMERECORDED int,NUMTRAPS int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index TRAPRECEIVER_STATS_N3 on TRAPRECEIVER_STATS (TIMERECORDED);"

sqli "CREATE TABLE TRAPRECEIVER_RT (ITEMID int, FROMTIME int, TOTIME int, NUMTRAPS_HIGH int, NUMTRAPS_AVG  int, NUMTRAPS_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index TRAPRECEIVER_RT_N3 on TRAPRECEIVER_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE TRAPRECEIVER_BL (ITEMID   int,TIMESLOT int,NUMTRAPS_HIGH int,NUMTRAPS_AVG  int,NUMTRAPS_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TRAPRECEIVER_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TRAPRECEIVER_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = SNMP Trap Receiver

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
