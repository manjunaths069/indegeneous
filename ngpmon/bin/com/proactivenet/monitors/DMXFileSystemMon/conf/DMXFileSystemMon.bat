REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'DMXFILESYSTEM_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adapter for Tivoli Unix FileSystem
REM Schema & PruneCntl Statements for Monitor = Adapter for Tivoli Unix FileSystem ,moTypeId = 31273

echo Creating tables DMXFILESYSTEM_STATS, DMXFILESYSTEM_RT, DMXFILESYSTEM_BL
echo Creating Prune Control entries for DMXFILESYSTEM_STATS  AND  entries for DMXFILESYSTEM_RT
sqli "CREATE TABLE DMXFILESYSTEM_STATS (ITEMID int,TIMERECORDED int,DMX_DSK_USD int,DMX_INODE_USD int,DMX_FS_AVAIL int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index DMXFILESYSTEM_STATS_N3 on DMXFILESYSTEM_STATS (TIMERECORDED);"

sqli "CREATE TABLE DMXFILESYSTEM_RT (ITEMID int, FROMTIME int, TOTIME int, DMX_DSK_USD_HIGH int, DMX_DSK_USD_AVG  int, DMX_DSK_USD_LOW  int, DMX_INODE_USD_HIGH int, DMX_INODE_USD_AVG  int, DMX_INODE_USD_LOW  int, DMX_FS_AVAIL_HIGH int, DMX_FS_AVAIL_AVG  int, DMX_FS_AVAIL_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index DMXFILESYSTEM_RT_N3 on DMXFILESYSTEM_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE DMXFILESYSTEM_BL (ITEMID   int,TIMESLOT int,STARTTIME int,DURATION int,DMX_DSK_USD_HIGH int,DMX_DSK_USD_AVG  int,DMX_DSK_USD_LOW  int,DMX_INODE_USD_HIGH int,DMX_INODE_USD_AVG  int,DMX_INODE_USD_LOW  int,DMX_FS_AVAIL_HIGH int,DMX_FS_AVAIL_AVG  int,DMX_FS_AVAIL_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "STARTTIME", "DURATION") WITH HASH SIZE 10);"

sqli "create index DMXFILESYSTEM_BL_N2 on DMXFILESYSTEM_BL ( STARTTIME, DURATION );"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('DMXFILESYSTEM_STATS', 'TIMERECORDED', 192);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('DMXFILESYSTEM_RT', 'FROMTIME', 2160);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('DMXFILESYSTEM_BL', 'STARTTIME', 8760);"

) else (

REM Upgrade Installation for Monitor = Adapter for Tivoli Unix FileSystem

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)