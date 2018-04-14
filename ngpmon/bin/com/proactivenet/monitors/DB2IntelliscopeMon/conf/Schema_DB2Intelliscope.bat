REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'DB2INTELLISCOPE_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = DB2 Intelliscope
REM Schema & PruneCntl Statements for Monitor = DB2 Intelliscope ,moTypeId = 48011

echo Creating tables DB2INTELLISCOPE_STATS, DB2INTELLISCOPE_RT, DB2INTELLISCOPE_BL
echo Creating Prune Control entries for DB2INTELLISCOPE_STATS  AND  entries for DB2INTELLISCOPE_RT
sqli "CREATE TABLE DB2INTELLISCOPE_STATS (ITEMID int,TIMERECORDED int,HIT_RATIO int,IDX_HIT_RATIO int,PKG_CHE_LU int,PKG_CHE_IN int,PKG_CHE_HR int,CTG_CHE_LU int,CTG_CHE_IN int,CTG_CHE_HR int,LOG_DATA_RD int,PHY_DATA_RD int,DATA_WR int,LOG_IDX_RD int,PHY_IDX_RD int,IDX_WR int,ASYNC_DATA_RD int,ASYNC_DATA_WR int,ASYNC_IDX_WR int,ASYNC_WR int,RANDOM_RD int,LOCK_LIST_UMEM int,LOG_USED int,LOG_AVAIL int,LOCK_WAIT int,AVG_LW_TIME int,TOTAL_SORTS int,AVG_SORT_TIME int,ACT_SORT int,SORT_OVRFLO int,COMMITS int,ROLL_BACK int,DYN_SQL int,STAT_SQL int,SELECT_SQL int,DML_SQL int,BINDS int,RC_INSERT int,RC_UPDATE int,RC_DELETE int,RC_SELECT int,AUTO_RBINDS int,INT_COMMITS int,INT_ROLBAKS int,ROLBAKS_DL int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli -f %server_path%\pw\monitors\monitors\DB2IntelliscopeMon\DB2Intelliscope_schema_sql.txt



) else (

REM Upgrade Installation for Monitor = DB2 Intelliscope

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)
