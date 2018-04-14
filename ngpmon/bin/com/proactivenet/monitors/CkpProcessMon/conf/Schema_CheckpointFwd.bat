REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'FWPROC_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Intelliscope for VPN-1/FireWall-1: Process
REM Schema & PruneCntl Statements for Monitor = Intelliscope for VPN-1/FireWall-1: Process ,moTypeId = 20006

echo Creating tables FWPROC_STATS, FWPROC_RT, FWPROC_BL
echo Creating Prune Control entries for FWPROC_STATS  AND  entries for FWPROC_RT
sqli "CREATE TABLE FWPROC_STATS (ITEMID int,TIMERECORDED int,AVAILABILITY int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index FWPROC_STATS_N3 on FWPROC_STATS (TIMERECORDED);"

sqli "CREATE TABLE FWPROC_RT (ITEMID int, FROMTIME int, TOTIME int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index FWPROC_RT_N3 on FWPROC_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE FWPROC_BL (ITEMID   int,TIMESLOT int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('FWPROC_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('FWPROC_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Intelliscope for VPN-1/FireWall-1: Process

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
