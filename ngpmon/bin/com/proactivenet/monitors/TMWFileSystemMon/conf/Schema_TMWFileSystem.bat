REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'TMWFileSystem_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adapter for Tivoli Windows FileSystem
REM Schema & PruneCntl Statements for Monitor = Adapter for Tivoli Windows FileSystem ,moTypeId = 31255

echo Creating tables TMWFileSystem_STATS, TMWFileSystem_RT, TMWFileSystem_BL
echo Creating Prune Control entries for TMWFileSystem_STATS  AND  entries for TMWFileSystem_RT
sqli "CREATE TABLE TMWFileSystem_STATS (ITEMID int,TIMERECORDED int,USEDSPACE int,FSYSAVAIL int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index TMWFileSystem_STATS_N3 on TMWFileSystem_STATS (TIMERECORDED);"

sqli "CREATE TABLE TMWFileSystem_RT (ITEMID int, FROMTIME int, TOTIME int, USEDSPACE_HIGH int, USEDSPACE_AVG  int, USEDSPACE_LOW  int, FSYSAVAIL_HIGH int, FSYSAVAIL_AVG  int, FSYSAVAIL_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index TMWFileSystem_RT_N3 on TMWFileSystem_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE TMWFileSystem_BL (ITEMID   int,TIMESLOT int,STARTTIME int,DURATION int,USEDSPACE_HIGH int,USEDSPACE_AVG  int,USEDSPACE_LOW  int,FSYSAVAIL_HIGH int,FSYSAVAIL_AVG  int,FSYSAVAIL_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "STARTTIME", "DURATION") WITH HASH SIZE 10);"

sqli "create index TMWFileSystem_BL_N2 on TMWFileSystem_BL ( STARTTIME, DURATION );"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TMWFileSystem_STATS', 'TIMERECORDED', 192);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TMWFileSystem_RT', 'FROMTIME', 2160);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('TMWFileSystem_BL', 'STARTTIME', 8760);"

) else (

REM Upgrade Installation for Monitor = Adapter for Tivoli Windows FileSystem

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
