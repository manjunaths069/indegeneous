REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'NQ_DISKPERF_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adapter for NetIQ AppManager Disk Performance
REM Schema & PruneCntl Statements for Monitor = Adapter for NetIQ AppManager Disk Performance ,moTypeId = 40018

echo Creating tables NQ_DISKPERF_STATS, NQ_DISKPERF_RT, NQ_DISKPERF_BL
echo Creating Prune Control entries for NQ_DISKPERF_STATS  AND  entries for NQ_DISKPERF_RT
sqli "CREATE TABLE NQ_DISKPERF_STATS (ITEMID int,TIMERECORDED int,DATA_AVAIL int,AVAILABILITY int,DISK_Q_LEN int,DISK_BUSY int,DISK_READ_TIME int,DISK_WRITE_TIME int,DISK_READ_QLEN int,DISK_WRITE_QLEN int,DISK_READS int,DISK_WRITES int,DISK_TRAN_TIME int,SRVC_TIME int,READS int,WRITES int,READ_BYTES int,WRITE_BYTES int,TRN_WAIT_TIME int,TRN_WAIT_Q int,WAIT_TIME int,TRANSFERS int,TRANSFER_BYTES int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index NQ_DISKPERF_STATS_N3 on NQ_DISKPERF_STATS (TIMERECORDED);"

sqli "CREATE TABLE NQ_DISKPERF_RT (ITEMID int, FROMTIME int, TOTIME int, DATA_AVAIL_HIGH int, DATA_AVAIL_AVG  int, DATA_AVAIL_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, DISK_Q_LEN_HIGH int, DISK_Q_LEN_AVG  int, DISK_Q_LEN_LOW  int, DISK_BUSY_HIGH int, DISK_BUSY_AVG  int, DISK_BUSY_LOW  int, DISK_READ_TIME_HIGH int, DISK_READ_TIME_AVG  int, DISK_READ_TIME_LOW  int, DISK_WRITE_TIME_HIGH int, DISK_WRITE_TIME_AVG  int, DISK_WRITE_TIME_LOW  int, DISK_READ_QLEN_HIGH int, DISK_READ_QLEN_AVG  int, DISK_READ_QLEN_LOW  int, DISK_WRITE_QLEN_HIGH int, DISK_WRITE_QLEN_AVG  int, DISK_WRITE_QLEN_LOW  int, DISK_READS_HIGH int, DISK_READS_AVG  int, DISK_READS_LOW  int, DISK_WRITES_HIGH int, DISK_WRITES_AVG  int, DISK_WRITES_LOW  int, DISK_TRAN_TIME_HIGH int, DISK_TRAN_TIME_AVG  int, DISK_TRAN_TIME_LOW  int, SRVC_TIME_HIGH int, SRVC_TIME_AVG  int, SRVC_TIME_LOW  int, READS_HIGH int, READS_AVG  int, READS_LOW  int, WRITES_HIGH int, WRITES_AVG  int, WRITES_LOW  int, READ_BYTES_HIGH int, READ_BYTES_AVG  int, READ_BYTES_LOW  int, WRITE_BYTES_HIGH int, WRITE_BYTES_AVG  int, WRITE_BYTES_LOW  int, TRN_WAIT_TIME_HIGH int, TRN_WAIT_TIME_AVG  int, TRN_WAIT_TIME_LOW  int, TRN_WAIT_Q_HIGH int, TRN_WAIT_Q_AVG  int, TRN_WAIT_Q_LOW  int, WAIT_TIME_HIGH int, WAIT_TIME_AVG  int, WAIT_TIME_LOW  int, TRANSFERS_HIGH int, TRANSFERS_AVG  int, TRANSFERS_LOW  int, TRANSFER_BYTES_HIGH int, TRANSFER_BYTES_AVG  int, TRANSFER_BYTES_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index NQ_DISKPERF_RT_N3 on NQ_DISKPERF_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE NQ_DISKPERF_BL (ITEMID   int,TIMESLOT int,STARTTIME int,DURATION int,DATA_AVAIL_HIGH int,DATA_AVAIL_AVG  int,DATA_AVAIL_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,DISK_Q_LEN_HIGH int,DISK_Q_LEN_AVG  int,DISK_Q_LEN_LOW  int,DISK_BUSY_HIGH int,DISK_BUSY_AVG  int,DISK_BUSY_LOW  int,DISK_READ_TIME_HIGH int,DISK_READ_TIME_AVG  int,DISK_READ_TIME_LOW  int,DISK_WRITE_TIME_HIGH int,DISK_WRITE_TIME_AVG  int,DISK_WRITE_TIME_LOW  int,DISK_READ_QLEN_HIGH int,DISK_READ_QLEN_AVG  int,DISK_READ_QLEN_LOW  int,DISK_WRITE_QLEN_HIGH int,DISK_WRITE_QLEN_AVG  int,DISK_WRITE_QLEN_LOW  int,DISK_READS_HIGH int,DISK_READS_AVG  int,DISK_READS_LOW  int,DISK_WRITES_HIGH int,DISK_WRITES_AVG  int,DISK_WRITES_LOW  int,DISK_TRAN_TIME_HIGH int,DISK_TRAN_TIME_AVG  int,DISK_TRAN_TIME_LOW  int,SRVC_TIME_HIGH int,SRVC_TIME_AVG  int,SRVC_TIME_LOW  int,READS_HIGH int,READS_AVG  int,READS_LOW  int,WRITES_HIGH int,WRITES_AVG  int,WRITES_LOW  int,READ_BYTES_HIGH int,READ_BYTES_AVG  int,READ_BYTES_LOW  int,WRITE_BYTES_HIGH int,WRITE_BYTES_AVG  int,WRITE_BYTES_LOW  int,TRN_WAIT_TIME_HIGH int,TRN_WAIT_TIME_AVG  int,TRN_WAIT_TIME_LOW  int,TRN_WAIT_Q_HIGH int,TRN_WAIT_Q_AVG  int,TRN_WAIT_Q_LOW  int,WAIT_TIME_HIGH int,WAIT_TIME_AVG  int,WAIT_TIME_LOW  int,TRANSFERS_HIGH int,TRANSFERS_AVG  int,TRANSFERS_LOW  int,TRANSFER_BYTES_HIGH int,TRANSFER_BYTES_AVG  int,TRANSFER_BYTES_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "STARTTIME", "DURATION") WITH HASH SIZE 10);"

sqli "create index NQ_DISKPERF_BL_N2 on NQ_DISKPERF_BL ( STARTTIME, DURATION );"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_DISKPERF_STATS', 'TIMERECORDED', 192);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_DISKPERF_RT', 'FROMTIME', 2160);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_DISKPERF_BL', 'STARTTIME', 8760);"

) else (

REM Upgrade Installation for Monitor = Adapter for NetIQ AppManager Disk Performance

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
