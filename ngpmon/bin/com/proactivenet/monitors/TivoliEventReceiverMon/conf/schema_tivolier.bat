REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'TIVOLI_EVENT_REC_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Event Receiver for Tivoli
REM Schema & PruneCntl Statements for Monitor = Event Receiver for Tivoli ,moTypeId = 31241

echo Creating tables TIVOLI_EVENT_REC_STATS, TIVOLI_EVENT_REC_RT, TIVOLI_EVENT_REC_BL
echo Creating Prune Control entries for TIVOLI_EVENT_REC_STATS  AND  entries for TIVOLI_EVENT_REC_RT
sqli "CREATE TABLE TIVOLI_EVENT_REC_STATS (ITEMID int,TIMERECORDED int,EVENT_COUNT int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index TIVOLI_EVENT_REC_STATS_N3 on TIVOLI_EVENT_REC_STATS (TIMERECORDED);"

sqli "CREATE TABLE TIVOLI_EVENT_REC_RT (ITEMID int, FROMTIME int, TOTIME int, EVENT_COUNT_HIGH int, EVENT_COUNT_AVG  int, EVENT_COUNT_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index TIVOLI_EVENT_REC_RT_N3 on TIVOLI_EVENT_REC_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE TIVOLI_EVENT_REC_BL (ITEMID   int,TIMESLOT int,EVENT_COUNT_HIGH int,EVENT_COUNT_AVG  int,EVENT_COUNT_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TIVOLI_EVENT_REC_STATS', 'TIMERECORDED', 192);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TIVOLI_EVENT_REC_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Event Receiver for Tivoli

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
