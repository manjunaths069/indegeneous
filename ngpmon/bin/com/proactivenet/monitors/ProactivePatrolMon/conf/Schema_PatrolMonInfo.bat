REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'BMC_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Adaptor for BMC PATROL Single Parameter
REM Schema & PruneCntl Statements for Monitor = Adaptor for BMC PATROL Single Parameter ,moTypeId = 29753

echo Creating tables BMC_STATS, BMC_RT, BMC_BL
echo Creating Prune Control entries for BMC_STATS  AND  entries for BMC_RT
sqli "CREATE TABLE BMC_STATS (ITEMID int,TIMERECORDED int,VAL int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index BMC_STATS_N3 on BMC_STATS (TIMERECORDED);"

sqli "CREATE TABLE BMC_RT (ITEMID int, FROMTIME int, TOTIME int, VAL_HIGH int, VAL_AVG  int, VAL_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index BMC_RT_N3 on BMC_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE BMC_BL (ITEMID   int,TIMESLOT int,VAL_HIGH int,VAL_AVG  int,VAL_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BMC_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BMC_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Adaptor for BMC PATROL Single Parameter

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
