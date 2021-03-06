REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'AIXSYSTEM_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = AIX System
REM Schema & PruneCntl Statements for Monitor = AIX System ,moTypeId = 41501

echo Creating tables AIXSYSTEM_STATS, AIXSYSTEM_RT, AIXSYSTEM_BL
echo Creating Prune Control entries for AIXSYSTEM_STATS  AND  entries for AIXSYSTEM_RT
sqli "CREATE TABLE AIXSYSTEM_STATS (ITEMID int,TIMERECORDED int,PROCESSES int,IOWAIT int,USERCPU int,KERNELCPU int,TOTALCPU int,AVGRUNQ int,AVGSWPQ int,AVGPROCWAIT int,SCANRATE int,MEMUSEDPCT int,SWAPUSEDPCT int,SYSLOAD int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index AIXSYSTEM_STATS_N3 on AIXSYSTEM_STATS (TIMERECORDED);"

sqli "CREATE TABLE AIXSYSTEM_RT (ITEMID int, FROMTIME int, TOTIME int, PROCESSES_HIGH int, PROCESSES_AVG  int, PROCESSES_LOW  int, IOWAIT_HIGH int, IOWAIT_AVG  int, IOWAIT_LOW  int, USERCPU_HIGH int, USERCPU_AVG  int, USERCPU_LOW  int, KERNELCPU_HIGH int, KERNELCPU_AVG  int, KERNELCPU_LOW  int, TOTALCPU_HIGH int, TOTALCPU_AVG  int, TOTALCPU_LOW  int, AVGRUNQ_HIGH int, AVGRUNQ_AVG  int, AVGRUNQ_LOW  int, AVGSWPQ_HIGH int, AVGSWPQ_AVG  int, AVGSWPQ_LOW  int, AVGPROCWAIT_HIGH int, AVGPROCWAIT_AVG  int, AVGPROCWAIT_LOW  int, SCANRATE_HIGH int, SCANRATE_AVG  int, SCANRATE_LOW  int, MEMUSEDPCT_HIGH int, MEMUSEDPCT_AVG  int, MEMUSEDPCT_LOW  int, SWAPUSEDPCT_HIGH int, SWAPUSEDPCT_AVG  int, SWAPUSEDPCT_LOW  int, SYSLOAD_HIGH int, SYSLOAD_AVG  int, SYSLOAD_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index AIXSYSTEM_RT_N3 on AIXSYSTEM_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE AIXSYSTEM_BL (ITEMID   int,TIMESLOT int,PROCESSES_HIGH int,PROCESSES_AVG  int,PROCESSES_LOW  int,IOWAIT_HIGH int,IOWAIT_AVG  int,IOWAIT_LOW  int,USERCPU_HIGH int,USERCPU_AVG  int,USERCPU_LOW  int,KERNELCPU_HIGH int,KERNELCPU_AVG  int,KERNELCPU_LOW  int,TOTALCPU_HIGH int,TOTALCPU_AVG  int,TOTALCPU_LOW  int,AVGRUNQ_HIGH int,AVGRUNQ_AVG  int,AVGRUNQ_LOW  int,AVGSWPQ_HIGH int,AVGSWPQ_AVG  int,AVGSWPQ_LOW  int,AVGPROCWAIT_HIGH int,AVGPROCWAIT_AVG  int,AVGPROCWAIT_LOW  int,SCANRATE_HIGH int,SCANRATE_AVG  int,SCANRATE_LOW  int,MEMUSEDPCT_HIGH int,MEMUSEDPCT_AVG  int,MEMUSEDPCT_LOW  int,SWAPUSEDPCT_HIGH int,SWAPUSEDPCT_AVG  int,SWAPUSEDPCT_LOW  int,SYSLOAD_HIGH int,SYSLOAD_AVG  int,SYSLOAD_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('AIXSYSTEM_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('AIXSYSTEM_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = AIX System

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
