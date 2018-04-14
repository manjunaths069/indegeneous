REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'BVIMCACHE_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = BroadVision Interaction Manager Cache
REM Schema & PruneCntl Statements for Monitor = BroadVision Interaction Manager Cache ,moTypeId = 27602

echo Creating tables BVIMCACHE_STATS, BVIMCACHE_RT, BVIMCACHE_BL
echo Creating Prune Control entries for BVIMCACHE_STATS  AND  entries for BVIMCACHE_RT
sqli "CREATE TABLE BVIMCACHE_STATS (ITEMID int,TIMERECORDED int,STSIZE int,HIT int,MISS int,SWAP int,PCTMISS int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index BVIMCACHE_STATS_N3 on BVIMCACHE_STATS (TIMERECORDED);"

sqli "CREATE TABLE BVIMCACHE_RT (ITEMID int, FROMTIME int, TOTIME int, STSIZE_HIGH int, STSIZE_AVG  int, STSIZE_LOW  int, HIT_HIGH int, HIT_AVG  int, HIT_LOW  int, MISS_HIGH int, MISS_AVG  int, MISS_LOW  int, SWAP_HIGH int, SWAP_AVG  int, SWAP_LOW  int, PCTMISS_HIGH int, PCTMISS_AVG  int, PCTMISS_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index BVIMCACHE_RT_N3 on BVIMCACHE_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE BVIMCACHE_BL (ITEMID   int,TIMESLOT int,STSIZE_HIGH int,STSIZE_AVG  int,STSIZE_LOW  int,HIT_HIGH int,HIT_AVG  int,HIT_LOW  int,MISS_HIGH int,MISS_AVG  int,MISS_LOW  int,SWAP_HIGH int,SWAP_AVG  int,SWAP_LOW  int,PCTMISS_HIGH int,PCTMISS_AVG  int,PCTMISS_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BVIMCACHE_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BVIMCACHE_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = BroadVision Interaction Manager Cache

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
