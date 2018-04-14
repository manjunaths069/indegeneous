REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'HPXFILESYSTEM_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = HPUX FileSystem 
REM Schema & PruneCntl Statements for Monitor = HPUX FileSystem  ,moTypeId = 28451

echo Creating tables HPXFILESYSTEM_STATS, HPXFILESYSTEM_RT, HPXFILESYSTEM_BL
echo Creating Prune Control entries for HPXFILESYSTEM_STATS  AND  entries for HPXFILESYSTEM_RT
sqli "CREATE TABLE HPXFILESYSTEM_STATS (ITEMID int,TIMERECORDED int,AVAILABILITY int,USEDPCT int,INODEUSEDPCT int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index HPXFILESYSTEM_STATS_N3 on HPXFILESYSTEM_STATS (TIMERECORDED);"

sqli "CREATE TABLE HPXFILESYSTEM_RT (ITEMID int, FROMTIME int, TOTIME int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, USEDPCT_HIGH int, USEDPCT_AVG  int, USEDPCT_LOW  int, INODEUSEDPCT_HIGH int, INODEUSEDPCT_AVG  int, INODEUSEDPCT_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index HPXFILESYSTEM_RT_N3 on HPXFILESYSTEM_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE HPXFILESYSTEM_BL (ITEMID   int,TIMESLOT int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,USEDPCT_HIGH int,USEDPCT_AVG  int,USEDPCT_LOW  int,INODEUSEDPCT_HIGH int,INODEUSEDPCT_AVG  int,INODEUSEDPCT_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('HPXFILESYSTEM_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('HPXFILESYSTEM_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = HPUX FileSystem 

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
