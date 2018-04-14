REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'ATGAPPSRV_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = ATG Dynamo
REM Schema & PruneCntl Statements for Monitor = ATG Dynamo ,moTypeId = 27101

echo Creating tables ATGAPPSRV_STATS, ATGAPPSRV_RT, ATGAPPSRV_BL
echo Creating Prune Control entries for ATGAPPSRV_STATS  AND  entries for ATGAPPSRV_RT
sqli "CREATE TABLE ATGAPPSRV_STATS (ITEMID int,TIMERECORDED int,AVGLATENCY int,FREEMEM int,REQPERMIN int,ODRPREQ int,DRPTHDWAITING int,AVAILABILITY int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index ATGAPPSRV_STATS_N3 on ATGAPPSRV_STATS (TIMERECORDED);"

sqli "CREATE TABLE ATGAPPSRV_RT (ITEMID int, FROMTIME int, TOTIME int, AVGLATENCY_HIGH int, AVGLATENCY_AVG  int, AVGLATENCY_LOW  int, FREEMEM_HIGH int, FREEMEM_AVG  int, FREEMEM_LOW  int, REQPERMIN_HIGH int, REQPERMIN_AVG  int, REQPERMIN_LOW  int, ODRPREQ_HIGH int, ODRPREQ_AVG  int, ODRPREQ_LOW  int, DRPTHDWAITING_HIGH int, DRPTHDWAITING_AVG  int, DRPTHDWAITING_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index ATGAPPSRV_RT_N3 on ATGAPPSRV_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE ATGAPPSRV_BL (ITEMID   int,TIMESLOT int,AVGLATENCY_HIGH int,AVGLATENCY_AVG  int,AVGLATENCY_LOW  int,FREEMEM_HIGH int,FREEMEM_AVG  int,FREEMEM_LOW  int,REQPERMIN_HIGH int,REQPERMIN_AVG  int,REQPERMIN_LOW  int,ODRPREQ_HIGH int,ODRPREQ_AVG  int,ODRPREQ_LOW  int,DRPTHDWAITING_HIGH int,DRPTHDWAITING_AVG  int,DRPTHDWAITING_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('ATGAPPSRV_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('ATGAPPSRV_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = ATG Dynamo

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
