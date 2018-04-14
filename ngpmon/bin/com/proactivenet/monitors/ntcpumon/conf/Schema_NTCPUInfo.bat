REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'NTCPU_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Windows NT/2000 CPU
REM Schema & PruneCntl Statements for Monitor = Windows NT/2000 CPU ,moTypeId = 29351

echo Creating tables NTCPU_STATS, NTCPU_RT, NTCPU_BL
echo Creating Prune Control entries for NTCPU_STATS  AND  entries for NTCPU_RT
sqli "CREATE TABLE NTCPU_STATS (ITEMID int,TIMERECORDED int,PPTIME int,PUTIME int,PPREVTIME int,INTERPSEC int,DPCRATE int,DPCQUEUEPSEC int,DPCBPASSPSEC int,PDPCTIME int,PINTERRUPTTIME int,APCQUEUEPSEC int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index NTCPU_STATS_N3 on NTCPU_STATS (TIMERECORDED);"

sqli "CREATE TABLE NTCPU_RT (ITEMID int, FROMTIME int, TOTIME int, PPTIME_HIGH int, PPTIME_AVG  int, PPTIME_LOW  int, PUTIME_HIGH int, PUTIME_AVG  int, PUTIME_LOW  int, PPREVTIME_HIGH int, PPREVTIME_AVG  int, PPREVTIME_LOW  int, INTERPSEC_HIGH int, INTERPSEC_AVG  int, INTERPSEC_LOW  int, DPCRATE_HIGH int, DPCRATE_AVG  int, DPCRATE_LOW  int, DPCQUEUEPSEC_HIGH int, DPCQUEUEPSEC_AVG  int, DPCQUEUEPSEC_LOW  int, DPCBPASSPSEC_HIGH int, DPCBPASSPSEC_AVG  int, DPCBPASSPSEC_LOW  int, PDPCTIME_HIGH int, PDPCTIME_AVG  int, PDPCTIME_LOW  int, PINTERRUPTTIME_HIGH int, PINTERRUPTTIME_AVG  int, PINTERRUPTTIME_LOW  int, APCQUEUEPSEC_HIGH int, APCQUEUEPSEC_AVG  int, APCQUEUEPSEC_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index NTCPU_RT_N3 on NTCPU_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE NTCPU_BL (ITEMID   int,TIMESLOT int,PPTIME_HIGH int,PPTIME_AVG  int,PPTIME_LOW  int,PUTIME_HIGH int,PUTIME_AVG  int,PUTIME_LOW  int,PPREVTIME_HIGH int,PPREVTIME_AVG  int,PPREVTIME_LOW  int,INTERPSEC_HIGH int,INTERPSEC_AVG  int,INTERPSEC_LOW  int,DPCRATE_HIGH int,DPCRATE_AVG  int,DPCRATE_LOW  int,DPCQUEUEPSEC_HIGH int,DPCQUEUEPSEC_AVG  int,DPCQUEUEPSEC_LOW  int,DPCBPASSPSEC_HIGH int,DPCBPASSPSEC_AVG  int,DPCBPASSPSEC_LOW  int,PDPCTIME_HIGH int,PDPCTIME_AVG  int,PDPCTIME_LOW  int,PINTERRUPTTIME_HIGH int,PINTERRUPTTIME_AVG  int,PINTERRUPTTIME_LOW  int,APCQUEUEPSEC_HIGH int,APCQUEUEPSEC_AVG  int,APCQUEUEPSEC_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NTCPU_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('NTCPU_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Windows NT/2000 CPU

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)