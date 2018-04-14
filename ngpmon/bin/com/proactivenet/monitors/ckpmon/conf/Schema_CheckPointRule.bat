REM Check if its a Clean or an Upgrade Installation

sqli -c "SELECT * FROM SYSTABLE B WHERE B.TABLE_NAME = 'CKPTFWONE_STATS';" > out.txt

if %errorlevel% == 101 (

REM Clean Installation for Monitor = Intelliscope for VPN-1/FireWall-1: Packet
REM Schema & PruneCntl Statements for Monitor = Intelliscope for VPN-1/FireWall-1: Packet ,moTypeId = 20001

echo Creating tables CKPTFWONE_STATS, CKPTFWONE_RT, CKPTFWONE_BL
echo Creating Prune Control entries for CKPTFWONE_STATS  AND  entries for CKPTFWONE_RT
sqli "CREATE TABLE CKPTFWONE_STATS (ITEMID int,TIMERECORDED int,PKTSACCEPTED int,PKTSREJECTED int,PKTSDROPPED int,PKTSENCRYPT int,AVAILABILITY int,CONNREQRCVD int,PKTSAUTHCRYPT int,PKTSAUTH int,PKTSDEAUTH int,PKTSDECRYPT int,PKTSKEYINST int,PRIMARY KEY ("ITEMID", "TIMERECORDED") WITH HASH SIZE 10);"

REM If the STATS table creation failed exit with errorcode
if %errorlevel% == 1 (
    exit /b 117
)

sqli "create        index CKPTFWONE_STATS_N3 on CKPTFWONE_STATS (TIMERECORDED);"

sqli "CREATE TABLE CKPTFWONE_RT (ITEMID int, FROMTIME int, TOTIME int, PKTSACCEPTED_HIGH int, PKTSACCEPTED_AVG  int, PKTSACCEPTED_LOW  int, PKTSREJECTED_HIGH int, PKTSREJECTED_AVG  int, PKTSREJECTED_LOW  int, PKTSDROPPED_HIGH int, PKTSDROPPED_AVG  int, PKTSDROPPED_LOW  int, PKTSENCRYPT_HIGH int, PKTSENCRYPT_AVG  int, PKTSENCRYPT_LOW  int, AVAILABILITY_HIGH int, AVAILABILITY_AVG  int, AVAILABILITY_LOW  int, CONNREQRCVD_HIGH int, CONNREQRCVD_AVG  int, CONNREQRCVD_LOW  int, PKTSAUTHCRYPT_HIGH int, PKTSAUTHCRYPT_AVG  int, PKTSAUTHCRYPT_LOW  int, PKTSAUTH_HIGH int, PKTSAUTH_AVG  int, PKTSAUTH_LOW  int, PKTSDEAUTH_HIGH int, PKTSDEAUTH_AVG  int, PKTSDEAUTH_LOW  int, PKTSDECRYPT_HIGH int, PKTSDECRYPT_AVG  int, PKTSDECRYPT_LOW  int, PKTSKEYINST_HIGH int, PKTSKEYINST_AVG  int, PKTSKEYINST_LOW  int, NUMPOINTS int, NUMSECS   int, PRIMARY KEY ("ITEMID", "FROMTIME") WITH HASH SIZE 10);"

sqli "create index CKPTFWONE_RT_N3 on CKPTFWONE_RT (FROMTIME,TOTIME);"

sqli "CREATE TABLE CKPTFWONE_BL (ITEMID   int,TIMESLOT int,PKTSACCEPTED_HIGH int,PKTSACCEPTED_AVG  int,PKTSACCEPTED_LOW  int,PKTSREJECTED_HIGH int,PKTSREJECTED_AVG  int,PKTSREJECTED_LOW  int,PKTSDROPPED_HIGH int,PKTSDROPPED_AVG  int,PKTSDROPPED_LOW  int,PKTSENCRYPT_HIGH int,PKTSENCRYPT_AVG  int,PKTSENCRYPT_LOW  int,AVAILABILITY_HIGH int,AVAILABILITY_AVG  int,AVAILABILITY_LOW  int,CONNREQRCVD_HIGH int,CONNREQRCVD_AVG  int,CONNREQRCVD_LOW  int,PKTSAUTHCRYPT_HIGH int,PKTSAUTHCRYPT_AVG  int,PKTSAUTHCRYPT_LOW  int,PKTSAUTH_HIGH int,PKTSAUTH_AVG  int,PKTSAUTH_LOW  int,PKTSDEAUTH_HIGH int,PKTSDEAUTH_AVG  int,PKTSDEAUTH_LOW  int,PKTSDECRYPT_HIGH int,PKTSDECRYPT_AVG  int,PKTSDECRYPT_LOW  int,PKTSKEYINST_HIGH int,PKTSKEYINST_AVG  int,PKTSKEYINST_LOW  int,NUMPOINTS int,NUMWEEKS int,PRIMARY KEY ("ITEMID", "TIMESLOT") WITH HASH SIZE 10);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('CKPTFWONE_STATS', 'TIMERECORDED', 24);"

sqli "INSERT INTO PRUNE_CNTL (TABLENAME, TIMECOLUMN, DELTAHOURS) VALUES ('CKPTFWONE_RT', 'FROMTIME', 2160);"

) else (

REM Upgrade Installation for Monitor = Intelliscope for VPN-1/FireWall-1: Packet

REM sqli -c "
REM SELECT * FROM SYSCOLUMN A, SYSTABLE B WHERE A.TABLE_ID = B.TABLE_ID AND B.TABLE_NAME = 'TABLENAME' AND A.COLUMN_NAME = 'COLUMNNAME';
REM "
REM if %errorlevel% == 101 (

REM alter table add column statement here
echo Upgrade installation

REM )

)