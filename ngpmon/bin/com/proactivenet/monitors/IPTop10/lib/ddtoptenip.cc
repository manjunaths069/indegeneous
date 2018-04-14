/*ddtoptenip.cc*/

#include <string.h>
#include <sys/types.h>


#include "ddtoptenip.h"



int IpTrafRecComp(IpTrafRec *First, IpTrafRec *Second){
    //int tot1 = 0;
    //int tot2 = 0;
    
    longlong_t tot1 = 0;
    longlong_t tot2 = 0;

    int result = -1;
    //printf("inside compare fn...");
    if ((First == NULL) || (Second == NULL))
    return 0;

    switch(Second->sort_on){

        case IP_ADDR:
                     //used for finding the node using IP address.
                     //printf("\nFirst IP:%s",First->ip_addr);
                     //printf("\nSecond IP:%s",Second->ip_addr);
                     //return strcmp(First->ip_addr,Second->ip_addr);
		     result = strcmp(First->ip_addr,Second->ip_addr) ;
		     if (result == 0)
		     {
			result = strcmp(First->nw_interface,Second->nw_interface);
		     }
            return result;
        case PKT_TOT:
                     //Sorting needs to be done in decending order
                     tot1 = First->pkts_frm + First->pkts_to;
                     tot2 = Second->pkts_frm + Second->pkts_to;
                     if(tot1 > tot2){
                         return -1;
                     }else if(tot1 < tot2){
                         return 1;
                     }

                     return 0;

        case BYTE_TOT:
                     //Sorting needs to be done in decending order
                     tot1 = First->bytes_frm + First->bytes_to;
                     tot2 = Second->bytes_frm + Second->bytes_to;
                     if(tot1 > tot2){
                         return -1;
                     }else if(tot1 < tot2){
                         return 1;
                     }

                     return 0;

    }

	return 0;
}



/*int main(){
    SortKey srt;
    srt =  PKT_TOT;
    if(srt == IP_ADDR){
        //printf("hello\n");
    }

    IpTrafRec *tr1,*tr2;

    tr1 = (IpTrafRec*)malloc(sizeof(IpTrafRec));
    tr2 = (IpTrafRec*)malloc(sizeof(IpTrafRec));


    tr1->bytes_frm = 192;
    tr1->bytes_to = 2;

    tr2->bytes_frm = 20;
    tr2->bytes_to = 2;

    tr2->sort_on = BYTE_TOT;

    printf("%d",IpTrafRecComp(tr1,tr2));

    free(tr1);
    free(tr2);

}*/
