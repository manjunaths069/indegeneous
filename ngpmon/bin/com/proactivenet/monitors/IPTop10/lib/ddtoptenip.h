/*ddtoptenip.h*/

#ifndef _DDTOPTENIP_H
#define _DDTOPTENIP_H

#pragma ident   "@(#)ddtoptenip.h"


/*Sortkey Enumerations*/
typedef enum sort_key{
    IP_ADDR   = 0x00F, //Used in node comparison to locate node.
    PKT_TOT   = 0x019, //To sort on packet total.
    BYTE_TOT  = 0x023  //To sort on byte total.
} SortKey;


/*structure to store details of each IP which make traffic
   to/from the given IP" */
/*typedef struct ip_traf_rec{
     char *ip_addr;
     unsigned long pkts_frm;
     unsigned long pkts_to;
     unsigned long bytes_frm;
     unsigned long bytes_to;
     SortKey sort_on;
} IpTrafRec;*/

typedef struct ip_traf_rec{
    char *ip_addr;
	//sojan start
	char *nw_interface;
	//sojan end
    longlong_t pkts_frm;
    longlong_t pkts_to;
    longlong_t bytes_frm;
    longlong_t bytes_to;
    SortKey sort_on;
} IpTrafRec;


/*Node compare function to be passed
to Linked list while list creation*/

int IpTrafRecComp(IpTrafRec *First, IpTrafRec *Second);

#endif /*End of ddtoptenip.h*/
