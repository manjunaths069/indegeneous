REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'NQ_FILESYS_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adapter for NetIQ AppManager FileSystem
REM Schema & PruneCntl Statements for Monitor = Adapter for NetIQ AppManager FileSystem ,moTypeId = 40016

echo Creating tables NQ_FILESYS_STATS, NQ_FILESYS_RT, NQ_FILESYS_BL
echo Creating Prune Control entries for NQ_FILESYS_STATS  AND  entries for NQ_FILESYS_RT
sqli "CREATE TABLE NQ_FILESYS_STATS (ITEMID int,TIMERECORDED int,DATA_AVAIL int,AVAILABILITY int,DISK_USED int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index NQ_FILESYS_STATS_N3 on NQ_FILESYS_STATS (TIMERECORDED);"

sqli "CREATE TABLE NQ_FILESYS_RT (ITEMID int, FROMTIME int, TOTIME int, DATA_AVAIL_HIGH int, DATA_AVAIL_AVG  int, DATA_AVAIL_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, DISK_USED_HIGH int, DISK_USED_AVG  int, DISK_USED_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index NQ_FILESYS_RT_N3 on NQ_FILESYS_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE NQ_FILESYS_BL (ITEMID   int,TIMESLOT int,STARTTIME int,DURATION int,DATA_AVAIL_HIGH int,DATA_AVAIL_AVG  int,DATA_AVAIL_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,DISK_USED_HIGH int,DISK_USED_AVG  int,DISK_USED_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_FILESYS_STATS', 'TIMERECORDED', 192);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_FILESYS_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Adapter for NetIQ AppManager FileSystem

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
