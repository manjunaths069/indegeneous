REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'INFORMIXQRY_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Informix Query
REM Schema & PruneCntl Statements for Monitor = Informix Query ,moTypeId = 25021

echo Creating tables INFORMIXQRY_STATS, INFORMIXQRY_RT, INFORMIXQRY_BL
echo Creating Prune Control entries for INFORMIXQRY_STATS  AND  entries for INFORMIXQRY_RT
sqli "CREATE TABLE INFORMIXQRY_STATS (ITEMID int,TIMERECORDED int,RESPTIME int,AVAILABILITY int,CONNTIME int,QUERYVAL int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index INFORMIXQRY_STATS_N3 on INFORMIXQRY_STATS (TIMERECORDED);"

sqli "CREATE TABLE INFORMIXQRY_RT (ITEMID int, FROMTIME int, TOTIME int, RESPTIME_HIGH int, RESPTIME_AVG  int, RESPTIME_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, CONNTIME_HIGH int, CONNTIME_AVG  int, CONNTIME_LOW  int, QUERYVAL_HIGH int, QUERYVAL_AVG  int, QUERYVAL_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index INFORMIXQRY_RT_N3 on INFORMIXQRY_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE INFORMIXQRY_BL (ITEMID   int,TIMESLOT int,RESPTIME_HIGH int,RESPTIME_AVG  int,RESPTIME_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,CONNTIME_HIGH int,CONNTIME_AVG  int,CONNTIME_LOW  int,QUERYVAL_HIGH int,QUERYVAL_AVG  int,QUERYVAL_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('INFORMIXQRY_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('INFORMIXQRY_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Informix Query

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
