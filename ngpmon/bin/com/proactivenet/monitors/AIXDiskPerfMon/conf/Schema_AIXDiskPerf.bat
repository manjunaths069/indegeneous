REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'AIXDISK_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = AIX Disk Performance
REM Schema & PruneCntl Statements for Monitor = AIX Disk Performance ,moTypeId = 43501

echo Creating tables AIXDISK_STATS, AIXDISK_RT, AIXDISK_BL
echo Creating Prune Control entries for AIXDISK_STATS  AND  entries for AIXDISK_RT
sqli "CREATE TABLE AIXDISK_STATS (ITEMID int,TIMERECORDED int,TRFERS int,TRFKB int,BUSYPCT int,KBREAD int,KBWRITE int,AVAILABILITY int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index AIXDISK_STATS_N3 on AIXDISK_STATS (TIMERECORDED);"

sqli "CREATE TABLE AIXDISK_RT (ITEMID int, FROMTIME int, TOTIME int, TRFERS_HIGH int, TRFERS_AVG  int, TRFERS_LOW  int, TRFKB_HIGH int, TRFKB_AVG  int, TRFKB_LOW  int, BUSYPCT_HIGH int, BUSYPCT_AVG  int, BUSYPCT_LOW  int, KBREAD_HIGH int, KBREAD_AVG  int, KBREAD_LOW  int, KBWRITE_HIGH int, KBWRITE_AVG  int, KBWRITE_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index AIXDISK_RT_N3 on AIXDISK_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE AIXDISK_BL (ITEMID   int,TIMESLOT int,TRFERS_HIGH int,TRFERS_AVG  int,TRFERS_LOW  int,TRFKB_HIGH int,TRFKB_AVG  int,TRFKB_LOW  int,BUSYPCT_HIGH int,BUSYPCT_AVG  int,BUSYPCT_LOW  int,KBREAD_HIGH int,KBREAD_AVG  int,KBREAD_LOW  int,KBWRITE_HIGH int,KBWRITE_AVG  int,KBWRITE_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('AIXDISK_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('AIXDISK_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = AIX Disk Performance

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
