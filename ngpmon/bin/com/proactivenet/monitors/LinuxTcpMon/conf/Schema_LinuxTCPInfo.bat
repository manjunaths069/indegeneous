REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'LNXTCP_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Linux TCP
REM Schema & PruneCntl Statements for Monitor = Linux TCP ,moTypeId = 28801

echo Creating tables LNXTCP_STATS, LNXTCP_RT, LNXTCP_BL
echo Creating Prune Control entries for LNXTCP_STATS  AND  entries for LNXTCP_RT
sqli "CREATE TABLE LNXTCP_STATS (ITEMID int,TIMERECORDED int,ACTIVEOPENS int,PASSIVEOPENS int,ATTEMPTSFAILS int,ESTABRESETS int,CURRESTABS int,INSEGS int,OUTSEGSS int,RETRANSSEGS int,INERRS int,OUTRSTS int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index LNXTCP_STATS_N3 on LNXTCP_STATS (TIMERECORDED);"

sqli "CREATE TABLE LNXTCP_RT (ITEMID int, FROMTIME int, TOTIME int, ACTIVEOPENS_HIGH int, ACTIVEOPENS_AVG  int, ACTIVEOPENS_LOW  int, PASSIVEOPENS_HIGH int, PASSIVEOPENS_AVG  int, PASSIVEOPENS_LOW  int, ATTEMPTSFAILS_HIGH int, ATTEMPTSFAILS_AVG  int, ATTEMPTSFAILS_LOW  int, ESTABRESETS_HIGH int, ESTABRESETS_AVG  int, ESTABRESETS_LOW  int, CURRESTABS_HIGH int, CURRESTABS_AVG  int, CURRESTABS_LOW  int, INSEGS_HIGH int, INSEGS_AVG  int, INSEGS_LOW  int, OUTSEGSS_HIGH int, OUTSEGSS_AVG  int, OUTSEGSS_LOW  int, RETRANSSEGS_HIGH int, RETRANSSEGS_AVG  int, RETRANSSEGS_LOW  int, INERRS_HIGH int, INERRS_AVG  int, INERRS_LOW  int, OUTRSTS_HIGH int, OUTRSTS_AVG  int, OUTRSTS_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index LNXTCP_RT_N3 on LNXTCP_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE LNXTCP_BL (ITEMID   int,TIMESLOT int,ACTIVEOPENS_HIGH int,ACTIVEOPENS_AVG  int,ACTIVEOPENS_LOW  int,PASSIVEOPENS_HIGH int,PASSIVEOPENS_AVG  int,PASSIVEOPENS_LOW  int,ATTEMPTSFAILS_HIGH int,ATTEMPTSFAILS_AVG  int,ATTEMPTSFAILS_LOW  int,ESTABRESETS_HIGH int,ESTABRESETS_AVG  int,ESTABRESETS_LOW  int,CURRESTABS_HIGH int,CURRESTABS_AVG  int,CURRESTABS_LOW  int,INSEGS_HIGH int,INSEGS_AVG  int,INSEGS_LOW  int,OUTSEGSS_HIGH int,OUTSEGSS_AVG  int,OUTSEGSS_LOW  int,RETRANSSEGS_HIGH int,RETRANSSEGS_AVG  int,RETRANSSEGS_LOW  int,INERRS_HIGH int,INERRS_AVG  int,INERRS_LOW  int,OUTRSTS_HIGH int,OUTRSTS_AVG  int,OUTRSTS_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('LNXTCP_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('LNXTCP_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Linux TCP

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
