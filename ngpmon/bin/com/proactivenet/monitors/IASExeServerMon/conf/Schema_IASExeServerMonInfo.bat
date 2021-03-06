REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'IASEXESERV_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = SunOne Application Server Exe Monitor
REM Schema & PruneCntl Statements for Monitor = SunOne Application Server Exe Monitor ,moTypeId = 29652

echo Creating tables IASEXESERV_STATS, IASEXESERV_RT, IASEXESERV_BL
echo Creating Prune Control entries for IASEXESERV_STATS  AND  entries for IASEXESERV_RT
sqli "CREATE TABLE IASEXESERV_STATS (ITEMID int,TIMERECORDED int,CPULOAD int,DISKIO int,MEMTHRASH int,REQQUEUED int,CACHRESULTS int,AVGEXTIME int,REQUESTS int,CUREQ int,REQWAIT int,REQREADY int,CURREQTHRD int,REQTHRDWAIT int,TOTTHREAD int,BYTESENT int,BYTERECV int,CURRSESS int,AVAILABILITY int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index IASEXESERV_STATS_N3 on IASEXESERV_STATS (TIMERECORDED);"

sqli "CREATE TABLE IASEXESERV_RT (ITEMID int, FROMTIME int, TOTIME int, CPULOAD_HIGH int, CPULOAD_AVG  int, CPULOAD_LOW  int, DISKIO_HIGH int, DISKIO_AVG  int, DISKIO_LOW  int, MEMTHRASH_HIGH int, MEMTHRASH_AVG  int, MEMTHRASH_LOW  int, REQQUEUED_HIGH int, REQQUEUED_AVG  int, REQQUEUED_LOW  int, CACHRESULTS_HIGH int, CACHRESULTS_AVG  int, CACHRESULTS_LOW  int, AVGEXTIME_HIGH int, AVGEXTIME_AVG  int, AVGEXTIME_LOW  int, REQUESTS_HIGH int, REQUESTS_AVG  int, REQUESTS_LOW  int, CUREQ_HIGH int, CUREQ_AVG  int, CUREQ_LOW  int, REQWAIT_HIGH int, REQWAIT_AVG  int, REQWAIT_LOW  int, REQREADY_HIGH int, REQREADY_AVG  int, REQREADY_LOW  int, CURREQTHRD_HIGH int, CURREQTHRD_AVG  int, CURREQTHRD_LOW  int, REQTHRDWAIT_HIGH int, REQTHRDWAIT_AVG  int, REQTHRDWAIT_LOW  int, TOTTHREAD_HIGH int, TOTTHREAD_AVG  int, TOTTHREAD_LOW  int, BYTESENT_HIGH int, BYTESENT_AVG  int, BYTESENT_LOW  int, BYTERECV_HIGH int, BYTERECV_AVG  int, BYTERECV_LOW  int, CURRSESS_HIGH int, CURRSESS_AVG  int, CURRSESS_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index IASEXESERV_RT_N3 on IASEXESERV_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE IASEXESERV_BL (ITEMID   int,TIMESLOT int,CPULOAD_HIGH int,CPULOAD_AVG  int,CPULOAD_LOW  int,DISKIO_HIGH int,DISKIO_AVG  int,DISKIO_LOW  int,MEMTHRASH_HIGH int,MEMTHRASH_AVG  int,MEMTHRASH_LOW  int,REQQUEUED_HIGH int,REQQUEUED_AVG  int,REQQUEUED_LOW  int,CACHRESULTS_HIGH int,CACHRESULTS_AVG  int,CACHRESULTS_LOW  int,AVGEXTIME_HIGH int,AVGEXTIME_AVG  int,AVGEXTIME_LOW  int,REQUESTS_HIGH int,REQUESTS_AVG  int,REQUESTS_LOW  int,CUREQ_HIGH int,CUREQ_AVG  int,CUREQ_LOW  int,REQWAIT_HIGH int,REQWAIT_AVG  int,REQWAIT_LOW  int,REQREADY_HIGH int,REQREADY_AVG  int,REQREADY_LOW  int,CURREQTHRD_HIGH int,CURREQTHRD_AVG  int,CURREQTHRD_LOW  int,REQTHRDWAIT_HIGH int,REQTHRDWAIT_AVG  int,REQTHRDWAIT_LOW  int,TOTTHREAD_HIGH int,TOTTHREAD_AVG  int,TOTTHREAD_LOW  int,BYTESENT_HIGH int,BYTESENT_AVG  int,BYTESENT_LOW  int,BYTERECV_HIGH int,BYTERECV_AVG  int,BYTERECV_LOW  int,CURRSESS_HIGH int,CURRSESS_AVG  int,CURRSESS_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('IASEXESERV_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('IASEXESERV_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = SunOne Application Server Exe Monitor

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
