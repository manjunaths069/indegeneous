#!/bin/sh 

#### This scripts inserts Fast Poll Period
#### and Fast Poll Tolerance Count to the DB
#### for the existing instances.
. /usr/pw/pronto/bin/.tmprofile

itemid=0
itemid=`/usr/pw/pronto/bin/sqli << EOF
select itemid from param_cntl where attrid=201993;
EOF`

#echo "ItemId string is " $itemid
for item in $itemid
do
  #echo "Item id is : " $item
  attrid=`/usr/pw/pronto/bin/sqli << EOF1
  select attrid from param_cntl where attrid=201218 and itemid=$item;
EOF1`
#echo "Attrid is : " $attrid
	if [ "ZZ$attrid" = "ZZ" ] ; then
	 `sqli << EOF
	  insert into param_cntl values ($item,201218,25,20);
	  insert into param_cntl values ($item,201219,26,0);
EOF`
	#else
	#echo "Row already exists for itemid: " $item
	fi
done
