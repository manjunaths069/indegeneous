REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'WSMQ_Q_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = WebSphereMQ Queue
REM Schema & PruneCntl Statements for Monitor = WebSphereMQ Queue ,moTypeId = 40009

echo Creating tables WSMQ_Q_STATS, WSMQ_Q_RT, WSMQ_Q_BL
echo Creating Prune Control entries for WSMQ_Q_STATS  AND  entries for WSMQ_Q_RT
sqli "CREATE TABLE WSMQ_Q_STATS (ITEMID int,TIMERECORDED int,AVAILABILITY int,Q_USED int,OPN_INPUT_CNT int,OPN_OUTPUT_CNT int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index WSMQ_Q_STATS_N3 on WSMQ_Q_STATS (TIMERECORDED);"

sqli "CREATE TABLE WSMQ_Q_RT (ITEMID int, FROMTIME int, TOTIME int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, Q_USED_HIGH int, Q_USED_AVG  int, Q_USED_LOW  int, OPN_INPUT_CNT_HIGH int, OPN_INPUT_CNT_AVG  int, OPN_INPUT_CNT_LOW  int, OPN_OUTPUT_CNT_HIGH int, OPN_OUTPUT_CNT_AVG  int, OPN_OUTPUT_CNT_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index WSMQ_Q_RT_N3 on WSMQ_Q_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE WSMQ_Q_BL (ITEMID   int,TIMESLOT int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,Q_USED_HIGH int,Q_USED_AVG  int,Q_USED_LOW  int,OPN_INPUT_CNT_HIGH int,OPN_INPUT_CNT_AVG  int,OPN_INPUT_CNT_LOW  int,OPN_OUTPUT_CNT_HIGH int,OPN_OUTPUT_CNT_AVG  int,OPN_OUTPUT_CNT_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('WSMQ_Q_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('WSMQ_Q_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = WebSphereMQ Queue

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
