REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'BASICOID_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = SNMP OID
REM Schema & PruneCntl Statements for Monitor = SNMP OID ,moTypeId = 29051

echo Creating tables BASICOID_STATS, BASICOID_RT, BASICOID_BL
echo Creating Prune Control entries for BASICOID_STATS  AND  entries for BASICOID_RT
sqli "CREATE TABLE BASICOID_STATS (ITEMID int,TIMERECORDED int,VALUE int,PMATCH int,NMATCH int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index BASICOID_STATS_N3 on BASICOID_STATS (TIMERECORDED);"

sqli "CREATE TABLE BASICOID_RT (ITEMID int, FROMTIME int, TOTIME int, VALUE_HIGH int, VALUE_AVG  int, VALUE_LOW  int, PMATCH_HIGH int, PMATCH_AVG  int, PMATCH_LOW  int, NMATCH_HIGH int, NMATCH_AVG  int, NMATCH_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index BASICOID_RT_N3 on BASICOID_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE BASICOID_BL (ITEMID   int,TIMESLOT int,VALUE_HIGH int,VALUE_AVG  int,VALUE_LOW  int,PMATCH_HIGH int,PMATCH_AVG  int,PMATCH_LOW  int,NMATCH_HIGH int,NMATCH_AVG  int,NMATCH_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BASICOID_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('BASICOID_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = SNMP OID

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
