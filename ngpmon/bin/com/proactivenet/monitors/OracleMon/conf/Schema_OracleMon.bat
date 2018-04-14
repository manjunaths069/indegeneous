REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'ORAINTSCOPE_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Oracle Intelliscope
REM Schema & PruneCntl Statements for Monitor = Oracle Intelliscope ,moTypeId = 25001

echo Creating tables ORAINTSCOPE_STATS, ORAINTSCOPE_RT, ORAINTSCOPE_BL
echo Creating Prune Control entries for ORAINTSCOPE_STATS  AND  entries for ORAINTSCOPE_RT
sqli "CREATE TABLE ORAINTSCOPE_STATS (ITEMID int,TIMERECORDED int,COMMITS int,SORTINMEM int,ROLLPERCMT int,RECURSECALLS int,OPENCUR int,FREEDB int,DISKINP int,DISKOPS int,ACTIVETRANS int,SVRMEMORY int,MEMUSED int,CACHEHITRATIO int,USERCONN int,TLSESSMEMORY int,PKSESSMEMORY int,DEADLOCKS int,ERRORS int,FREEMEMORY int,CPU_USAGE int,DIC_SCANS int,DIC_PGETMISSES int,DIC_GETS int,DIC_GETMISSES int,PSCAN_MISSES int,DIC_SCANMISSES int,DIC_MODI int,DIC_USAGE int,LIB_GETS int,LIB_GETMISSES int,LIB_PINS int,LIB_PINMISSES int,IVALIDATIONS int,PAR_EXE_RATIO int,PAR_RATE int,REDO_BLCKS int,REDO_WRITES int,TABLE_BLCKS int,TABLE_ROWS int,TABLE_SMALLTBL int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli -f %server_path%\pw\monitors\monitors\OracleMon\OracleMon_schema_sql.txt

) else (

REM Upgrade Installation for Monitor = Oracle Intelliscope

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
