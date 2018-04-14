REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'FILESZ_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = File Size
REM Schema & PruneCntl Statements for Monitor = File Size ,moTypeId = 27201

echo Creating tables FILESZ_STATS, FILESZ_RT, FILESZ_BL
echo Creating Prune Control entries for FILESZ_STATS  AND  entries for FILESZ_RT
sqli "CREATE TABLE FILESZ_STATS (ITEMID int,TIMERECORDED int,STSIZE int,SIZERT int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index FILESZ_STATS_N3 on FILESZ_STATS (TIMERECORDED);"

sqli "CREATE TABLE FILESZ_RT (ITEMID int, FROMTIME int, TOTIME int, STSIZE_HIGH int, STSIZE_AVG  int, STSIZE_LOW  int, SIZERT_HIGH int, SIZERT_AVG  int, SIZERT_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index FILESZ_RT_N3 on FILESZ_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE FILESZ_BL (ITEMID   int,TIMESLOT int,STSIZE_HIGH int,STSIZE_AVG  int,STSIZE_LOW  int,SIZERT_HIGH int,SIZERT_AVG  int,SIZERT_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('FILESZ_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('FILESZ_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = File Size

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
