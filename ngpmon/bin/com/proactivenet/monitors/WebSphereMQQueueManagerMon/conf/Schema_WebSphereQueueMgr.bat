REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'WSMQ_QMGR_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = WebSphereMQ QueueManager
REM Schema & PruneCntl Statements for Monitor = WebSphereMQ QueueManager ,moTypeId = 40011

echo Creating tables WSMQ_QMGR_STATS, WSMQ_QMGR_RT, WSMQ_QMGR_BL
echo Creating Prune Control entries for WSMQ_QMGR_STATS  AND  entries for WSMQ_QMGR_RT
sqli "CREATE TABLE WSMQ_QMGR_STATS (ITEMID int,TIMERECORDED int,AVAILABILITY int,RESP_TIME int,DEAD_Q_USED int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index WSMQ_QMGR_STATS_N3 on WSMQ_QMGR_STATS (TIMERECORDED);"

sqli "CREATE TABLE WSMQ_QMGR_RT (ITEMID int, FROMTIME int, TOTIME int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, RESP_TIME_HIGH int, RESP_TIME_AVG  int, RESP_TIME_LOW  int, DEAD_Q_USED_HIGH int, DEAD_Q_USED_AVG  int, DEAD_Q_USED_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index WSMQ_QMGR_RT_N3 on WSMQ_QMGR_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE WSMQ_QMGR_BL (ITEMID   int,TIMESLOT int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,RESP_TIME_HIGH int,RESP_TIME_AVG  int,RESP_TIME_LOW  int,DEAD_Q_USED_HIGH int,DEAD_Q_USED_AVG  int,DEAD_Q_USED_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('WSMQ_QMGR_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('WSMQ_QMGR_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = WebSphereMQ QueueManager

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
