REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'BVIMSERVER_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = BroadVision Interaction Manager Server
REM Schema & PruneCntl Statements for Monitor = BroadVision Interaction Manager Server ,moTypeId = 27601

echo Creating tables BVIMSERVER_STATS, BVIMSERVER_RT, BVIMSERVER_BL
echo Creating Prune Control entries for BVIMSERVER_STATS  AND  entries for BVIMSERVER_RT
sqli "CREATE TABLE BVIMSERVER_STATS (ITEMID int,TIMERECORDED int,SESS int,CONN int,THR int,CGI int,IDLQ int,AVAILABILITY int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index BVIMSERVER_STATS_N3 on BVIMSERVER_STATS (TIMERECORDED);"

sqli "CREATE TABLE BVIMSERVER_RT (ITEMID int, FROMTIME int, TOTIME int, SESS_HIGH int, SESS_AVG  int, SESS_LOW  int, CONN_HIGH int, CONN_AVG  int, CONN_LOW  int, THR_HIGH int, THR_AVG  int, THR_LOW  int, CGI_HIGH int, CGI_AVG  int, CGI_LOW  int, IDLQ_HIGH int, IDLQ_AVG  int, IDLQ_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index BVIMSERVER_RT_N3 on BVIMSERVER_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE BVIMSERVER_BL (ITEMID   int,TIMESLOT int,SESS_HIGH int,SESS_AVG  int,SESS_LOW  int,CONN_HIGH int,CONN_AVG  int,CONN_LOW  int,THR_HIGH int,THR_AVG  int,THR_LOW  int,CGI_HIGH int,CGI_AVG  int,CGI_LOW  int,IDLQ_HIGH int,IDLQ_AVG  int,IDLQ_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BVIMSERVER_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BVIMSERVER_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = BroadVision Interaction Manager Server

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
