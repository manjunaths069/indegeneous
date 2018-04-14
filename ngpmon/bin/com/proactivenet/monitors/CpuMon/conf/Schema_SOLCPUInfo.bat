REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'SOLCPU_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Solaris CPU
REM Schema & PruneCntl Statements for Monitor = Solaris CPU ,moTypeId = 29301

echo Creating tables SOLCPU_STATS, SOLCPU_RT, SOLCPU_BL
echo Creating Prune Control entries for SOLCPU_STATS  AND  entries for SOLCPU_RT
sqli "CREATE TABLE SOLCPU_STATS (ITEMID int,TIMERECORDED int,USRTIME int,SYSTIME int,WAITTIME int,INTERRUPTS int,SCANRATE int,CNTXTSWT int,INVOLCNTXTSWT int,SYSCALLS int,INTRTHREAD int,CROSSCALLS int,MGRTHREAD int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index SOLCPU_STATS_N3 on SOLCPU_STATS (TIMERECORDED);"

sqli "CREATE TABLE SOLCPU_RT (ITEMID int, FROMTIME int, TOTIME int, USRTIME_HIGH int, USRTIME_AVG  int, USRTIME_LOW  int, SYSTIME_HIGH int, SYSTIME_AVG  int, SYSTIME_LOW  int, WAITTIME_HIGH int, WAITTIME_AVG  int, WAITTIME_LOW  int, INTERRUPTS_HIGH int, INTERRUPTS_AVG  int, INTERRUPTS_LOW  int, SCANRATE_HIGH int, SCANRATE_AVG  int, SCANRATE_LOW  int, CNTXTSWT_HIGH int, CNTXTSWT_AVG  int, CNTXTSWT_LOW  int, INVOLCNTXTSWT_HIGH int, INVOLCNTXTSWT_AVG  int, INVOLCNTXTSWT_LOW  int, SYSCALLS_HIGH int, SYSCALLS_AVG  int, SYSCALLS_LOW  int, INTRTHREAD_HIGH int, INTRTHREAD_AVG  int, INTRTHREAD_LOW  int, CROSSCALLS_HIGH int, CROSSCALLS_AVG  int, CROSSCALLS_LOW  int, MGRTHREAD_HIGH int, MGRTHREAD_AVG  int, MGRTHREAD_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index SOLCPU_RT_N3 on SOLCPU_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE SOLCPU_BL (ITEMID   int,TIMESLOT int,USRTIME_HIGH int,USRTIME_AVG  int,USRTIME_LOW  int,SYSTIME_HIGH int,SYSTIME_AVG  int,SYSTIME_LOW  int,WAITTIME_HIGH int,WAITTIME_AVG  int,WAITTIME_LOW  int,INTERRUPTS_HIGH int,INTERRUPTS_AVG  int,INTERRUPTS_LOW  int,SCANRATE_HIGH int,SCANRATE_AVG  int,SCANRATE_LOW  int,CNTXTSWT_HIGH int,CNTXTSWT_AVG  int,CNTXTSWT_LOW  int,INVOLCNTXTSWT_HIGH int,INVOLCNTXTSWT_AVG  int,INVOLCNTXTSWT_LOW  int,SYSCALLS_HIGH int,SYSCALLS_AVG  int,SYSCALLS_LOW  int,INTRTHREAD_HIGH int,INTRTHREAD_AVG  int,INTRTHREAD_LOW  int,CROSSCALLS_HIGH int,CROSSCALLS_AVG  int,CROSSCALLS_LOW  int,MGRTHREAD_HIGH int,MGRTHREAD_AVG  int,MGRTHREAD_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('SOLCPU_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('SOLCPU_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Solaris CPU

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
