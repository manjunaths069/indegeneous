REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'HPXCPU_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = HPUX CPU
REM Schema & PruneCntl Statements for Monitor = HPUX CPU ,moTypeId = 29451

echo Creating tables HPXCPU_STATS, HPXCPU_RT, HPXCPU_BL
echo Creating Prune Control entries for HPXCPU_STATS  AND  entries for HPXCPU_RT
sqli "CREATE TABLE HPXCPU_STATS (ITEMID int,TIMERECORDED int,USRTIME int,SYSTIME int,WAITTIME int,INTERRUPTS int,IDLETIME int,SYSCALLS int,CNXTSWTCH int,FAULTS int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index HPXCPU_STATS_N3 on HPXCPU_STATS (TIMERECORDED);"

sqli "CREATE TABLE HPXCPU_RT (ITEMID int, FROMTIME int, TOTIME int, USRTIME_HIGH int, USRTIME_AVG  int, USRTIME_LOW  int, SYSTIME_HIGH int, SYSTIME_AVG  int, SYSTIME_LOW  int, WAITTIME_HIGH int, WAITTIME_AVG  int, WAITTIME_LOW  int, INTERRUPTS_HIGH int, INTERRUPTS_AVG  int, INTERRUPTS_LOW  int, IDLETIME_HIGH int, IDLETIME_AVG  int, IDLETIME_LOW  int, SYSCALLS_HIGH int, SYSCALLS_AVG  int, SYSCALLS_LOW  int, CNXTSWTCH_HIGH int, CNXTSWTCH_AVG  int, CNXTSWTCH_LOW  int, FAULTS_HIGH int, FAULTS_AVG  int, FAULTS_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index HPXCPU_RT_N3 on HPXCPU_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE HPXCPU_BL (ITEMID   int,TIMESLOT int,USRTIME_HIGH int,USRTIME_AVG  int,USRTIME_LOW  int,SYSTIME_HIGH int,SYSTIME_AVG  int,SYSTIME_LOW  int,WAITTIME_HIGH int,WAITTIME_AVG  int,WAITTIME_LOW  int,INTERRUPTS_HIGH int,INTERRUPTS_AVG  int,INTERRUPTS_LOW  int,IDLETIME_HIGH int,IDLETIME_AVG  int,IDLETIME_LOW  int,SYSCALLS_HIGH int,SYSCALLS_AVG  int,SYSCALLS_LOW  int,CNXTSWTCH_HIGH int,CNXTSWTCH_AVG  int,CNXTSWTCH_LOW  int,FAULTS_HIGH int,FAULTS_AVG  int,FAULTS_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('HPXCPU_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('HPXCPU_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = HPUX CPU

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
