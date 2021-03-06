REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'NQ_TCP_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adapter for NetIQ AppManager Tcp
REM Schema & PruneCntl Statements for Monitor = Adapter for NetIQ AppManager Tcp ,moTypeId = 44088

echo Creating tables NQ_TCP_STATS, NQ_TCP_RT, NQ_TCP_BL
echo Creating Prune Control entries for NQ_TCP_STATS  AND  entries for NQ_TCP_RT
sqli "CREATE TABLE NQ_TCP_STATS (ITEMID int,TIMERECORDED int,DATA_AVAIL int,CONN_ESTB int,CONN_FAIL int,CONN_ACT int,CONN_PASS int,CONN_RSET int,SEG_RECV int,SEG_R_TRN int,SEG_SENT int,TOT_SEG int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index NQ_TCP_STATS_N3 on NQ_TCP_STATS (TIMERECORDED);"

sqli "CREATE TABLE NQ_TCP_RT (ITEMID int, FROMTIME int, TOTIME int, DATA_AVAIL_HIGH int, DATA_AVAIL_AVG  int, DATA_AVAIL_LOW  int, CONN_ESTB_HIGH int, CONN_ESTB_AVG  int, CONN_ESTB_LOW  int, CONN_FAIL_HIGH int, CONN_FAIL_AVG  int, CONN_FAIL_LOW  int, CONN_ACT_HIGH int, CONN_ACT_AVG  int, CONN_ACT_LOW  int, CONN_PASS_HIGH int, CONN_PASS_AVG  int, CONN_PASS_LOW  int, CONN_RSET_HIGH int, CONN_RSET_AVG  int, CONN_RSET_LOW  int, SEG_RECV_HIGH int, SEG_RECV_AVG  int, SEG_RECV_LOW  int, SEG_R_TRN_HIGH int, SEG_R_TRN_AVG  int, SEG_R_TRN_LOW  int, SEG_SENT_HIGH int, SEG_SENT_AVG  int, SEG_SENT_LOW  int, TOT_SEG_HIGH int, TOT_SEG_AVG  int, TOT_SEG_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index NQ_TCP_RT_N3 on NQ_TCP_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE NQ_TCP_BL (ITEMID   int,TIMESLOT int,STARTTIME int,DURATION int,DATA_AVAIL_HIGH int,DATA_AVAIL_AVG  int,DATA_AVAIL_LOW  int,CONN_ESTB_HIGH int,CONN_ESTB_AVG  int,CONN_ESTB_LOW  int,CONN_FAIL_HIGH int,CONN_FAIL_AVG  int,CONN_FAIL_LOW  int,CONN_ACT_HIGH int,CONN_ACT_AVG  int,CONN_ACT_LOW  int,CONN_PASS_HIGH int,CONN_PASS_AVG  int,CONN_PASS_LOW  int,CONN_RSET_HIGH int,CONN_RSET_AVG  int,CONN_RSET_LOW  int,SEG_RECV_HIGH int,SEG_RECV_AVG  int,SEG_RECV_LOW  int,SEG_R_TRN_HIGH int,SEG_R_TRN_AVG  int,SEG_R_TRN_LOW  int,SEG_SENT_HIGH int,SEG_SENT_AVG  int,SEG_SENT_LOW  int,TOT_SEG_HIGH int,TOT_SEG_AVG  int,TOT_SEG_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "STARTTIME", "DURATION") WITH HASH SIZE 10);"

sqli "create index NQ_TCP_BL_N2 on NQ_TCP_BL ( STARTTIME, DURATION );"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_TCP_STATS', 'TIMERECORDED', 192);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_TCP_RT', 'FROMTIME', 2160);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NQ_TCP_BL', 'STARTTIME', 8760);"

) else (

REM Upgrade Installation for Monitor = Adapter for NetIQ AppManager Tcp

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
