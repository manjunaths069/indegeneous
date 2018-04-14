REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'LOGFILE_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Log File
REM Schema & PruneCntl Statements for Monitor = Log File ,moTypeId = 27051

echo Creating tables LOGFILE_STATS, LOGFILE_RT, LOGFILE_BL
echo Creating Prune Control entries for LOGFILE_STATS  AND  entries for LOGFILE_RT
sqli "CREATE TABLE LOGFILE_STATS (ITEMID int,TIMERECORDED int,OCCURANCES int,SIZERT int,STSIZE int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index LOGFILE_STATS_N3 on LOGFILE_STATS (TIMERECORDED);"

sqli "CREATE TABLE LOGFILE_RT (ITEMID int, FROMTIME int, TOTIME int, OCCURANCES_HIGH int, OCCURANCES_AVG  int, OCCURANCES_LOW  int, SIZERT_HIGH int, SIZERT_AVG  int, SIZERT_LOW  int, STSIZE_HIGH int, STSIZE_AVG  int, STSIZE_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index LOGFILE_RT_N3 on LOGFILE_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE LOGFILE_BL (ITEMID   int,TIMESLOT int,OCCURANCES_HIGH int,OCCURANCES_AVG  int,OCCURANCES_LOW  int,SIZERT_HIGH int,SIZERT_AVG  int,SIZERT_LOW  int,STSIZE_HIGH int,STSIZE_AVG  int,STSIZE_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('LOGFILE_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('LOGFILE_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Log File

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
