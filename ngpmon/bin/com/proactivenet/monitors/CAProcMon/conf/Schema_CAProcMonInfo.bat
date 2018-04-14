REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'CAPROC_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adaptor for CA-NSM Process
REM Schema & PruneCntl Statements for Monitor = Adaptor for CA-NSM Process ,moTypeId = 29851

echo Creating tables CAPROC_STATS, CAPROC_RT, CAPROC_BL
echo Creating Prune Control entries for CAPROC_STATS  AND  entries for CAPROC_RT
sqli "CREATE TABLE CAPROC_STATS (ITEMID int,TIMERECORDED int,PROCSZ int,TOTALCPU int,INSTANCES int,AVAILABILITY int,TOTTHRDS int,STATUS int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index CAPROC_STATS_N3 on CAPROC_STATS (TIMERECORDED);"

sqli "CREATE TABLE CAPROC_RT (ITEMID int, FROMTIME int, TOTIME int, PROCSZ_HIGH int, PROCSZ_AVG  int, PROCSZ_LOW  int, TOTALCPU_HIGH int, TOTALCPU_AVG  int, TOTALCPU_LOW  int, INSTANCES_HIGH int, INSTANCES_AVG  int, INSTANCES_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, TOTTHRDS_HIGH int, TOTTHRDS_AVG  int, TOTTHRDS_LOW  int, STATUS_HIGH int, STATUS_AVG  int, STATUS_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index CAPROC_RT_N3 on CAPROC_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE CAPROC_BL (ITEMID   int,TIMESLOT int,PROCSZ_HIGH int,PROCSZ_AVG  int,PROCSZ_LOW  int,TOTALCPU_HIGH int,TOTALCPU_AVG  int,TOTALCPU_LOW  int,INSTANCES_HIGH int,INSTANCES_AVG  int,INSTANCES_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,TOTTHRDS_HIGH int,TOTTHRDS_AVG  int,TOTTHRDS_LOW  int,STATUS_HIGH int,STATUS_AVG  int,STATUS_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('CAPROC_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('CAPROC_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Adaptor for CA-NSM Process

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)