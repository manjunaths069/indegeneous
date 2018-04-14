REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'SES_WAIT_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Oracle Session Wait
REM Schema & PruneCntl Statements for Monitor = Oracle Session Wait ,moTypeId = 41024

echo Creating tables SES_WAIT_STATS, SES_WAIT_RT, SES_WAIT_BL
echo Creating Prune Control entries for SES_WAIT_STATS  AND  entries for SES_WAIT_RT
sqli "CREATE TABLE SES_WAIT_STATS (ITEMID int,TIMERECORDED int,Sess_wt_so_far int,SessCount int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index SES_WAIT_STATS_N3 on SES_WAIT_STATS (TIMERECORDED);"

sqli "CREATE TABLE SES_WAIT_RT (ITEMID int, FROMTIME int, TOTIME int, Sess_wt_so_far_HIGH int, Sess_wt_so_far_AVG  int, Sess_wt_so_far_LOW  int, SessCount_HIGH int, SessCount_AVG  int, SessCount_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index SES_WAIT_RT_N3 on SES_WAIT_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE SES_WAIT_BL (ITEMID   int,TIMESLOT int,Sess_wt_so_far_HIGH int,Sess_wt_so_far_AVG  int,Sess_wt_so_far_LOW  int,SessCount_HIGH int,SessCount_AVG  int,SessCount_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('SES_WAIT_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('SES_WAIT_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Oracle Session Wait

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
