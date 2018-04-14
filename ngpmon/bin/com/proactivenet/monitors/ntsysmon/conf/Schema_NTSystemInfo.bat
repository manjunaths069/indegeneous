REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'NTSYSTEM_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Windows NT/2000 System
REM Schema & PruneCntl Statements for Monitor = Windows NT/2000 System ,moTypeId = 21003

echo Creating tables NTSYSTEM_STATS, NTSYSTEM_RT, NTSYSTEM_BL
echo Creating Prune Control entries for NTSYSTEM_STATS  AND  entries for NTSYSTEM_RT
sqli "CREATE TABLE NTSYSTEM_STATS (ITEMID int,TIMERECORDED int,TOTALCPU int,SWAPUSEDPCT int,TOTALPROCS int,SYSTEMCPU int,USERCPU int,REALMEM int,INTERRUPTS int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index NTSYSTEM_STATS_N3 on NTSYSTEM_STATS (TIMERECORDED);"

sqli "CREATE TABLE NTSYSTEM_RT (ITEMID int, FROMTIME int, TOTIME int, TOTALCPU_HIGH int, TOTALCPU_AVG  int, TOTALCPU_LOW  int, SWAPUSEDPCT_HIGH int, SWAPUSEDPCT_AVG  int, SWAPUSEDPCT_LOW  int, TOTALPROCS_HIGH int, TOTALPROCS_AVG  int, TOTALPROCS_LOW  int, SYSTEMCPU_HIGH int, SYSTEMCPU_AVG  int, SYSTEMCPU_LOW  int, USERCPU_HIGH int, USERCPU_AVG  int, USERCPU_LOW  int, REALMEM_HIGH int, REALMEM_AVG  int, REALMEM_LOW  int, INTERRUPTS_HIGH int, INTERRUPTS_AVG  int, INTERRUPTS_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index NTSYSTEM_RT_N3 on NTSYSTEM_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE NTSYSTEM_BL (ITEMID   int,TIMESLOT int,TOTALCPU_HIGH int,TOTALCPU_AVG  int,TOTALCPU_LOW  int,SWAPUSEDPCT_HIGH int,SWAPUSEDPCT_AVG  int,SWAPUSEDPCT_LOW  int,TOTALPROCS_HIGH int,TOTALPROCS_AVG  int,TOTALPROCS_LOW  int,SYSTEMCPU_HIGH int,SYSTEMCPU_AVG  int,SYSTEMCPU_LOW  int,USERCPU_HIGH int,USERCPU_AVG  int,USERCPU_LOW  int,REALMEM_HIGH int,REALMEM_AVG  int,REALMEM_LOW  int,INTERRUPTS_HIGH int,INTERRUPTS_AVG  int,INTERRUPTS_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NTSYSTEM_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NTSYSTEM_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Windows NT/2000 System

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
