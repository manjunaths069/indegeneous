REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'LATCH_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Oracle Latch Monitor
REM Schema & PruneCntl Statements for Monitor = Oracle Latch Monitor ,moTypeId = 41006

echo Creating tables LATCH_STATS, LATCH_RT, LATCH_BL
echo Creating Prune Control entries for LATCH_STATS  AND  entries for LATCH_RT
sqli "CREATE TABLE LATCH_STATS (ITEMID int,TIMERECORDED int,Gets int,Misses int,Sleeps int,Immediate_Gets int,Imediate_Misses int,Spin_Gets int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index LATCH_STATS_N3 on LATCH_STATS (TIMERECORDED);"

sqli "CREATE TABLE LATCH_RT (ITEMID int, FROMTIME int, TOTIME int, Gets_HIGH int, Gets_AVG  int, Gets_LOW  int, Misses_HIGH int, Misses_AVG  int, Misses_LOW  int, Sleeps_HIGH int, Sleeps_AVG  int, Sleeps_LOW  int, Immediate_Gets_HIGH int, Immediate_Gets_AVG  int, Immediate_Gets_LOW  int, Imediate_Misses_HIGH int, Imediate_Misses_AVG  int, Imediate_Misses_LOW  int, Spin_Gets_HIGH int, Spin_Gets_AVG  int, Spin_Gets_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index LATCH_RT_N3 on LATCH_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE LATCH_BL (ITEMID   int,TIMESLOT int,Gets_HIGH int,Gets_AVG  int,Gets_LOW  int,Misses_HIGH int,Misses_AVG  int,Misses_LOW  int,Sleeps_HIGH int,Sleeps_AVG  int,Sleeps_LOW  int,Immediate_Gets_HIGH int,Immediate_Gets_AVG  int,Immediate_Gets_LOW  int,Imediate_Misses_HIGH int,Imediate_Misses_AVG  int,Imediate_Misses_LOW  int,Spin_Gets_HIGH int,Spin_Gets_AVG  int,Spin_Gets_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('LATCH_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('LATCH_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Oracle Latch Monitor

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
