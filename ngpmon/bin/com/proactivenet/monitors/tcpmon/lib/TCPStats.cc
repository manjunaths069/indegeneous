#include <stdio.h>
#include "TCPStats.h"

TCPStats::TCPStats()
{
/* ===================================================*/
        mtcpActiveOpens = 0;
        mtcpPassiveOpens = 0;
        mtcpCurrEstab = 0;
        mtcpListenDrop = 0;
     mtcpHalfOpenDrop = 0;
     mtcpListenDropQ0 = 0;
     mtcpInSegs = 0;
     mtcpInDataInorderSegs = 0;
     mtcpInDataUnorderSegs = 0;
     mtcpInDataDupSegs = 0;
     mtcpInDataPartDupSegs = 0;
     mtcpInBytes = 0;
     mtcpInDataInorderBytes = 0;
     mtcpInDataUnorderBytes = 0;
     mtcpInDataDupBytes = 0;
     mtcpInDataPartDupBytes = 0;
     mtcpOutSegs = 0;
     mtcpOutDataSegs = 0;
     mtcpRetransSegs = 0;
     mtcpOutBytes = 0;
     mtcpOutDataBytes = 0;
     mtcpRetransBytes = 0;
     mtcpAttemptFails = 0;
     mtcpOutRsts = 0;
/* ===================================================*/
     lasttcpActiveOpens = 0;
     lasttcpPassiveOpens = 0;
     lasttcpCurrEstab = 0;
     lasttcpListenDrop = 0;
     lasttcpHalfOpenDrop = 0;
     lasttcpListenDropQ0 = 0;
     lasttcpInSegs = 0;
     lasttcpInDataInorderSegs = 0;
     lasttcpInDataUnorderSegs = 0;
     lasttcpInDataDupSegs = 0;
     lasttcpInDataPartDupSegs = 0;
     lasttcpInBytes = 0;
     lasttcpInDataInorderBytes = 0;
     lasttcpInDataUnorderBytes = 0;
     lasttcpInDataDupBytes = 0;
     lasttcpInDataPartDupBytes = 0;
     lasttcpOutSegs = 0;
     lasttcpOutDataSegs = 0;
     lasttcpRetransSegs = 0;
     lasttcpOutBytes = 0;
     lasttcpOutDataBytes = 0;
     lasttcpRetransBytes = 0;
     lasttcpAttemptFails = 0;
     lasttcpOutRsts = 0;
/* ===================================================*/
     difftcpActiveOpens = 0;
     difftcpPassiveOpens = 0;
     difftcpCurrEstab = 0;
     difftcpListenDrop = 0;
     difftcpHalfOpenDrop = 0;
     difftcpListenDropQ0 = 0;
     difftcpInSegs = 0;
     difftcpInDataInorderSegs = 0;
     difftcpInDataUnorderSegs = 0;
     difftcpInDataDupSegs = 0;
     difftcpInDataPartDupSegs = 0;
     difftcpInBytes = 0;
     difftcpInDataInorderBytes = 0;
     difftcpInDataUnorderBytes = 0;
     difftcpInDataDupBytes = 0;
     difftcpInDataPartDupBytes = 0;
     difftcpOutSegs = 0;
     difftcpOutDataSegs = 0;
     difftcpRetransSegs = 0;
     difftcpOutBytes = 0;
     difftcpOutDataBytes = 0;
     difftcpRetransBytes = 0;
     difftcpAttemptFails = 0;
     difftcpOutRsts = 0;
/* ===================================================*/



}
void TCPStats::getMibData(mib2_tcp_t  *tcp_struct,JNIEnv * env, jobject obj)
{
        int     mib_sd;
        int     n;
        int             flags;
        void    *p;
        char    buf[BUFSIZ];
        struct  strbuf  control;
        struct  strbuf  data;
        // mib2_tcp_t      tcp_struct;

        struct  T_optmgmt_req *req_opt = (struct T_optmgmt_req * ) buf;
        struct  T_optmgmt_ack *ack_opt = (struct T_optmgmt_ack * ) buf;
        struct  T_error_ack *err_opt = (struct T_error_ack * ) buf;
        struct  opthdr   *req_hdr;

        /* -------------------- JNI Logging changes  START-------------------- */
        char mLogString[255];
        sprintf(mLogString,"Native Logging: function getMibData: START");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */


        mib_sd = open( DEV_TCP, O_RDWR );

        if( mib_sd == -1 )
        {

                /* -------------------- JNI Logging changes  START-------------------- */
                sprintf(mLogString,"Native Logging: function getMibData: Open function failed Error code %d",errno);
                logErr(env,obj,mLogString);
                /* -------------------- JNI Logging changes  END-------------------- */

                cout << "Open failed!!!" << endl;
                exit;
        }


/*
                int popTest = 0;
        popTest = ioctl( mib_sd, I_POP, &n );
                if( popTest == -1 )
                {
                        cout << "I_POP failed" << endl;
                        cout << "popTest = " << popTest << endl;
                        cout << "POP-ERRNO = " << errno << endl;
                        return;
                }
*/

        while( ioctl( mib_sd, I_POP, &n ) != -1 );

        int pushTest = 0;

        pushTest = ioctl(mib_sd, I_PUSH, TCP_MODULE);

        if( pushTest == -1 ) {
                cout << "PUSH  failed!!!" << endl;
                cout << "pushTest = " << pushTest << endl;
                cout << "POP-ERRNO = " << errno << endl;

                /* -------------------- JNI Logging changes  START-------------------- */
                sprintf(mLogString,"Native Logging: function getMibData: ioctl function failed Error code %d",errno);
                logErr(env,obj,mLogString);
                /* -------------------- JNI Logging changes  END-------------------- */

                return;
        }

        req_opt->PRIM_type = T_OPTMGMT_REQ;
        req_opt->OPT_offset = sizeof( struct T_optmgmt_req );
        req_opt->OPT_length = sizeof( struct opthdr );
        req_opt->MGMT_flags = T_CURRENT;

        req_hdr = (struct opthdr *) & req_opt[1];
        req_hdr->level  = MIB2_TCP;
        req_hdr->name   = 0;
        req_hdr->len    = 0;

        int     putFlag = 0;

        control.buf     =       buf;
        control.len     =       req_opt->OPT_length + req_opt->OPT_offset;

        putFlag = putmsg( mib_sd, &control, NULL, RS_HIPRI );

        if( putFlag == -1 ) {
                cout << "PUTMESG  failed!!!" << endl;
                cout << "ERRNO = " << errno << endl;
                cout << "putFlag = " << putFlag << endl;
                /* -------------------- JNI Logging changes  START-------------------- */
                sprintf(mLogString,"Native Logging: function getMibData: putmsg function failed Error code %d",errno);
                logErr(env,obj,mLogString);
                /* -------------------- JNI Logging changes  END-------------------- */

                return;
        }

        req_hdr = ( struct opthdr *) & ack_opt[1];

        control.maxlen = sizeof buf;

        p       =       tcp_struct;

        flags           = 0;

        n = getmsg(mib_sd, &control, NULL, &flags );

        data.maxlen     = req_hdr->len;
        data.buf        = (char *) p;
        data.len        = 0;
        flags           = 0;

        n = getmsg(mib_sd, 0, &data, &flags );

        close(mib_sd);

                mtcpActiveOpens                 =       (tcp_struct->tcpActiveOpens);
        mtcpPassiveOpens                =       tcp_struct->tcpPassiveOpens;
                mtcpCurrEstab                   =       tcp_struct->tcpCurrEstab;
                mtcpListenDrop                  =       tcp_struct->tcpListenDrop ;

        mtcpHalfOpenDrop                =       tcp_struct->tcpHalfOpenDrop;
                mtcpListenDropQ0                =   tcp_struct->tcpListenDropQ0;

        mtcpInSegs                              =   tcp_struct->tcpInSegs;
        mtcpInDataInorderSegs   =       tcp_struct->tcpInDataInorderSegs;
        mtcpInDataUnorderSegs   =   tcp_struct->tcpInDataUnorderSegs;
        mtcpInDataDupSegs               =   tcp_struct->tcpInDataDupSegs;
        mtcpInDataPartDupSegs   =   tcp_struct->tcpInDataPartDupSegs;

        mtcpInDataInorderBytes  =   tcp_struct->tcpInDataInorderBytes;
        mtcpInDataUnorderBytes  =   tcp_struct->tcpInDataUnorderBytes;
        mtcpInDataDupBytes              =   tcp_struct->tcpInDataDupBytes;
        mtcpInDataPartDupBytes  =   tcp_struct->tcpInDataPartDupBytes;

                // add up all the in BYTES

        mtcpInBytes                             =   mtcpInDataInorderBytes + mtcpInDataUnorderBytes + mtcpInDataDupBytes + mtcpInDataPartDupBytes;

        mtcpOutSegs                             =   tcp_struct->tcpOutSegs;
        mtcpOutDataSegs                 =   tcp_struct->tcpOutDataSegs;
        mtcpRetransSegs                 =   tcp_struct->tcpRetransSegs;

        mtcpOutDataBytes                =   tcp_struct->tcpOutDataBytes;
        mtcpRetransBytes                =   tcp_struct->tcpRetransBytes;

                // add up the Output BYTES

         mtcpOutBytes                   =   mtcpOutDataBytes + mtcpRetransBytes;

        mtcpAttemptFails                = tcp_struct->tcpAttemptFails;
        mtcpOutRsts                     = tcp_struct->tcpOutRsts;
        /* -------------------- JNI Logging changes  START-------------------- */
        sprintf(mLogString,"Native Logging: function getMibData: END");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */

        return;
}

void
TCPStats::printMTcpStats()
{
        cout << "printMTcpSttats!!" << endl;

        cout << "mtcpActiveOpens = " << mtcpActiveOpens << endl;
        cout << "mtcpPassiveOpens = " << mtcpPassiveOpens << endl;
        cout << "mtcpListenDrop = " << mtcpListenDrop << endl;
        cout << "mtcpCurrEstab = " << mtcpCurrEstab << endl;
        cout << "mtcpHalfOpenDrop = " << mtcpHalfOpenDrop << endl;
        cout << "mtcpListenDropQ0 = " << mtcpListenDropQ0 << endl;
        cout << "mtcpInSegs = " << mtcpInSegs << endl;
        cout << "mtcpInDataInorderSegs = " << mtcpInDataInorderSegs << endl;
        cout << "mtcpInDataUnorderSegs = " << mtcpInDataUnorderSegs << endl;
        cout << "mtcpInDataDupSegs = " << mtcpInDataDupSegs << endl;
        cout << "mtcpInDataPartDupSegs = " << mtcpInDataPartDupSegs << endl;
        cout << "mtcpInBytes = " << mtcpInBytes << endl;
        cout << "mtcpInDataInorderBytes = " << mtcpInDataInorderBytes << endl;
        cout << "mtcpInDataUnorderBytes = " << mtcpInDataUnorderBytes << endl;
        cout << "mtcpInDataDupBytes = " << mtcpInDataDupBytes  << endl;
        cout << "mtcpInDataPartDupBytes = " << mtcpInDataPartDupBytes << endl;
        cout << "mtcpOutSegs = " << mtcpOutSegs << endl;
        cout << "mtcpOutDataSegs = " << mtcpOutDataSegs << endl;
        cout << "mtcpRetransSegs = " << mtcpRetransSegs << endl;
        cout << "mtcpOutBytes = " << mtcpOutBytes << endl;
        cout << "mtcpOutDataBytes = " << mtcpOutDataBytes << endl;
        cout << "mtcpRetransBytes = " << mtcpRetransBytes << endl;

}

void
TCPStats::printLastTcpStats()
{
        cout << "printLastTcpStats!!" << endl;

        cout << "lasttcpActiveOpens = " << lasttcpActiveOpens << endl;
        cout << "lasttcpPassiveOpens = " << lasttcpPassiveOpens << endl;
        cout << "lasttcpListenDrop = " << lasttcpListenDrop << endl;
        cout << "lasttcpCurrEstab = " << lasttcpCurrEstab << endl;
        cout << "lasttcpHalfOpenDrop = " << lasttcpHalfOpenDrop << endl;
        cout << "lasttcpListenDropQ0 = " << lasttcpListenDropQ0 << endl;
        cout << "lasttcpInSegs = " << lasttcpInSegs << endl;
        cout << "lasttcpInDataInorderSegs = " << lasttcpInDataInorderSegs << endl;
        cout << "lasttcpInDataUnorderSegs = " << lasttcpInDataUnorderSegs << endl;
        cout << "lasttcpInDataDupSegs = " << lasttcpInDataDupSegs << endl;
        cout << "lasttcpInDataPartDupSegs = " << lasttcpInDataPartDupSegs << endl;
        cout << "lasttcpInBytes = " << lasttcpInBytes << endl;
        cout << "lasttcpInDataInorderBytes = " << lasttcpInDataInorderBytes << endl;
        cout << "lasttcpInDataUnorderBytes = " << lasttcpInDataUnorderBytes << endl;
        cout << "lasttcpInDataDupBytes = " << lasttcpInDataDupBytes  << endl;
        cout << "lasttcpInDataPartDupBytes = " << lasttcpInDataPartDupBytes << endl;
        cout << "lasttcpOutSegs = " << lasttcpOutSegs << endl;
        cout << "lasttcpOutDataSegs = " << lasttcpOutDataSegs << endl;
        cout << "lasttcpRetransSegs = " << lasttcpRetransSegs << endl;
        cout << "lasttcpOutBytes = " << lasttcpOutBytes << endl;
        cout << "lasttcpOutDataBytes = " << lasttcpOutDataBytes << endl;
        cout << "lasttcpRetransBytes = " << lasttcpRetransBytes << endl;

}

void
TCPStats::printDiffTcpStats()
{

        cout << "START printDiffTcpStats!!" << endl;

        cout << "#####################################################" << endl;

                cout << "mtcpActiveOpens    = " << mtcpActiveOpens << endl;
                cout << "lasttcpActiveOpens = " << lasttcpActiveOpens << endl;
        cout << "difftcpActiveOpens = " << difftcpActiveOpens << endl;

                cout << "#####################################################" << endl;

                cout << "mtcpPassiveOpens    = " << mtcpPassiveOpens << endl;
                cout << "lasttcpPassiveOpens = " << lasttcpPassiveOpens << endl;
        cout << "difftcpPassiveOpens = " << difftcpPassiveOpens << endl;

                cout << "#####################################################" << endl;
                cout << "mtcpListenDrop    = " << mtcpListenDrop << endl;
                cout << "lasttcpListenDrop = " << lasttcpListenDrop << endl;
        cout << "difftcpListenDrop = " << difftcpListenDrop << endl;
                cout << "#####################################################" << endl;

                cout << "mtcpCurrEstab    = " << mtcpCurrEstab << endl;
                cout << "lasttcpCurrEstab = " << lasttcpCurrEstab << endl;
        cout << "difftcpCurrEstab = " << difftcpCurrEstab << endl;
                cout << "#####################################################" << endl;

                cout << "mtcpHalfOpenDrop    = " << mtcpHalfOpenDrop << endl;
                cout << "lasttcpHalfOpenDrop = " << lasttcpHalfOpenDrop << endl;
        cout << "difftcpHalfOpenDrop = " << difftcpHalfOpenDrop << endl;
                cout << "#####################################################" << endl;

                cout << "mtcpListenDropQ0    = " << mtcpListenDropQ0 << endl;
                cout << "lasttcpListenDropQ0 = " << lasttcpListenDropQ0 << endl;
        cout << "difftcpListenDropQ0 = " << difftcpListenDropQ0 << endl;
                cout << "#####################################################" << endl;

                cout << "mtcpInSegs    = " << mtcpInSegs << endl;
                cout << "lasttcpInSegs = " << lasttcpInSegs << endl;
        cout << "difftcpInSegs = " << difftcpInSegs << endl;
                cout << "#####################################################" << endl;

                cout << "mtcpInDataInorderSegs    = " << mtcpInDataInorderSegs << endl;
                cout << "lasttcpInDataInorderSegs = " << lasttcpInDataInorderSegs << endl;
        cout << "difftcpInDataInorderSegs = " << difftcpInDataInorderSegs << endl;
                cout << "#####################################################" << endl;

                cout << "mtcpInDataUnorderSegs    = " << mtcpInDataUnorderSegs << endl;
                cout << "lasttcpInDataUnorderSegs = " << lasttcpInDataUnorderSegs << endl;
        cout << "difftcpInDataUnorderSegs = " << difftcpInDataUnorderSegs << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpInDataDupSegs    = " << mtcpInDataDupSegs << endl;
        cout << "lasttcpInDataDupSegs = " << lasttcpInDataDupSegs << endl;
        cout << "difftcpInDataDupSegs = " << difftcpInDataDupSegs << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpInDataPartDupSegs    = " << mtcpInDataPartDupSegs << endl;
        cout << "lasttcpInDataPartDupSegs = " << lasttcpInDataPartDupSegs << endl;
        cout << "difftcpInDataPartDupSegs = " << difftcpInDataPartDupSegs << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpInBytes    = " << mtcpInBytes << endl;
        cout << "lasttcpInBytes = " << lasttcpInBytes << endl;
        cout << "difftcpInBytes = " << difftcpInBytes << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpInDataInorderBytes    = " << mtcpInDataInorderBytes << endl;
        cout << "lasttcpInDataInorderBytes = " << lasttcpInDataInorderBytes << endl;
        cout << "difftcpInDataInorderBytes = " << difftcpInDataInorderBytes << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpInDataUnorderBytes    = " << mtcpInDataUnorderBytes << endl;
        cout << "lasttcpInDataUnorderBytes = " << lasttcpInDataUnorderBytes << endl;
        cout << "difftcpInDataUnorderBytes = " << difftcpInDataUnorderBytes << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpInDataDupBytes    = " << mtcpInDataDupBytes  << endl;
        cout << "lasttcpInDataDupBytes = " << lasttcpInDataDupBytes  << endl;
        cout << "difftcpInDataDupBytes = " << difftcpInDataDupBytes  << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpInDataPartDupBytes    = " << mtcpInDataPartDupBytes << endl;
        cout << "lasttcpInDataPartDupBytes = " << lasttcpInDataPartDupBytes << endl;
        cout << "difftcpInDataPartDupBytes = " << difftcpInDataPartDupBytes << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpOutSegs    = " << mtcpOutSegs << endl;
        cout << "lasttcpOutSegs = " << lasttcpOutSegs << endl;
        cout << "difftcpOutSegs = " << difftcpOutSegs << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpOutDataSegs    = " << mtcpOutDataSegs << endl;
        cout << "lasttcpOutDataSegs = " << lasttcpOutDataSegs << endl;
        cout << "difftcpOutDataSegs = " << difftcpOutDataSegs << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpRetransSegs    = " << mtcpRetransSegs << endl;
        cout << "lasttcpRetransSegs = " << lasttcpRetransSegs << endl;
        cout << "difftcpRetransSegs = " << difftcpRetransSegs << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpOutBytes    = " << mtcpOutBytes << endl;
        cout << "lasttcpOutBytes = " << lasttcpOutBytes << endl;
        cout << "difftcpOutBytes = " << difftcpOutBytes << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpOutDataBytes    = " << mtcpOutDataBytes << endl;
        cout << "lasttcpOutDataBytes = " << lasttcpOutDataBytes << endl;
        cout << "difftcpOutDataBytes = " << difftcpOutDataBytes << endl;
                cout << "#####################################################" << endl;

        cout << "mtcpRetransBytes    = " << mtcpRetransBytes << endl;
        cout << "lasttcpRetransBytes = " << lasttcpRetransBytes << endl;
        cout << "difftcpRetransBytes = " << difftcpRetransBytes << endl;
                cout << "#####################################################" << endl;

        cout << "END printDiffTcpStats!!" << endl;

}


void
TCPStats::updataLast(JNIEnv * env,jobject obj)
{
        /* -------------------- JNI Logging changes  START-------------------- */
        char mLogString[255];
        sprintf(mLogString,"Native Logging: function updatalast: START");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */

        lasttcpActiveOpens                      =       mtcpActiveOpens;
        lasttcpPassiveOpens             =       mtcpPassiveOpens;
        lasttcpListenDrop                       =       mtcpListenDrop;
        lasttcpCurrEstab                        =       mtcpCurrEstab;
        lasttcpHalfOpenDrop             =       mtcpHalfOpenDrop;
        lasttcpListenDropQ0             =       mtcpListenDropQ0;
        lasttcpInSegs                           =       mtcpInSegs;
        lasttcpInDataInorderSegs        =       mtcpInDataInorderSegs;
        lasttcpInDataUnorderSegs        =       mtcpInDataUnorderSegs;
        lasttcpInDataDupSegs            =       mtcpInDataDupSegs;
        lasttcpInDataPartDupSegs        =       mtcpInDataPartDupSegs;
        lasttcpInBytes                          =       mtcpInBytes;
        lasttcpInDataInorderBytes       =       mtcpInDataInorderBytes;
        lasttcpInDataUnorderBytes       =       mtcpInDataUnorderBytes;
        lasttcpInDataDupBytes           =       mtcpInDataDupBytes;
        lasttcpInDataPartDupBytes       =       mtcpInDataPartDupBytes;
        lasttcpOutSegs                          =       mtcpOutSegs;
        lasttcpOutDataSegs                      =       mtcpOutDataSegs;
        lasttcpRetransSegs                      =       mtcpRetransSegs;
        lasttcpOutBytes                         =       mtcpOutBytes;
        lasttcpOutDataBytes             =       mtcpOutDataBytes;
        lasttcpRetransBytes             =       mtcpRetransBytes;
        lasttcpAttemptFails             =       mtcpAttemptFails;
        lasttcpOutRsts                  =       mtcpOutRsts;

        /* -------------------- JNI Logging changes  START-------------------- */
        sprintf(mLogString,"Native Logging: function updatalast: END");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */

}

void
TCPStats::getDiffTcpStats(JNIEnv * env,jobject obj)
{

        /* -------------------- JNI Logging changes  START-------------------- */
        char mLogString[255];
        sprintf(mLogString,"Native Logging: function getDiffTcpStats: START");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */


     difftcpActiveOpens =
                         mtcpActiveOpens - lasttcpActiveOpens ;

     difftcpPassiveOpens =
                         mtcpPassiveOpens - lasttcpPassiveOpens;

     difftcpCurrEstab =
                         mtcpCurrEstab;

     difftcpListenDrop =
                         mtcpListenDrop - lasttcpListenDrop;

     difftcpHalfOpenDrop =
                         mtcpHalfOpenDrop - lasttcpHalfOpenDrop;
     difftcpListenDropQ0 =
                         mtcpListenDropQ0 - lasttcpListenDropQ0;

     difftcpInSegs =
                         mtcpInSegs - lasttcpInSegs;

     difftcpInDataInorderSegs =
                         mtcpInDataInorderSegs - lasttcpInDataInorderSegs;

     difftcpInDataUnorderSegs =
                         mtcpInDataUnorderSegs - lasttcpInDataUnorderSegs;

     difftcpInDataDupSegs =
                         mtcpInDataDupSegs - lasttcpInDataDupSegs;

     difftcpInDataPartDupSegs =
                         mtcpInDataPartDupSegs - lasttcpInDataPartDupSegs;




     difftcpInBytes     =
                         ((mtcpInBytes - lasttcpInBytes ) / 1000);

     difftcpInDataInorderBytes =
                         ((mtcpInDataInorderBytes - lasttcpInDataInorderBytes) / 1000);

     difftcpInDataUnorderBytes =
                         ((mtcpInDataUnorderBytes - lasttcpInDataUnorderBytes) / 1000);

     difftcpInDataDupBytes =
                         ((mtcpInDataDupBytes - lasttcpInDataDupBytes) / 1000);

     difftcpInDataPartDupBytes =
                         ((mtcpInDataPartDupBytes - lasttcpInDataPartDupBytes) / 1000);

     difftcpOutSegs =
                         mtcpOutSegs - lasttcpOutSegs;

     difftcpOutDataSegs =
                         mtcpOutDataSegs - lasttcpOutDataSegs;

     difftcpRetransSegs =
                         mtcpRetransSegs - lasttcpRetransSegs;

     difftcpOutBytes =
                         ((mtcpOutBytes - lasttcpOutBytes) / 1000);

     difftcpOutDataBytes =
                         ((mtcpOutDataBytes - lasttcpOutDataBytes) / 1000);

     difftcpRetransBytes =
                         ((mtcpRetransBytes - lasttcpRetransBytes) / 1000);

     difftcpAttemptFails =
                         mtcpAttemptFails - lasttcpAttemptFails;

     difftcpOutRsts =
                        mtcpOutRsts - lasttcpOutRsts;
        /* -------------------- JNI Logging changes  START-------------------- */
         sprintf(mLogString,"Native Logging: function getDiffTcpStats: END");
        logDbg(env,obj,mLogString);
        /* -------------------- JNI Logging changes  END-------------------- */



}

/* --------------------- Changes for JNI logging START------------------------------*/

void TCPStats::logDbg(JNIEnv *env, jobject obj, char *logStr)
{
        if(logStr == NULL)
                return;
        char mlogStr[1280];
        strcpy(mlogStr,logStr);
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logDbg","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(mlogStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

void TCPStats::logErr(JNIEnv *env, jobject obj, char *errStr)
{
        if(errStr == NULL)
                return;
        char merrStr[1280];
        strcpy(merrStr,errStr);
        jclass cls = env->GetObjectClass(obj);
        jmethodID mid;
        if(cls != NULL)
        {
              mid = env->GetMethodID(cls,"logErr","(Ljava/lang/String;)V");
              if(mid != NULL)
              {
                      jstring tString = env->NewStringUTF(merrStr);
                      env->CallVoidMethod(obj,mid,tString);
                      env->DeleteLocalRef(tString);
              }
        }
}

/* --------------------- Changes for JNI logging END------------------------------*/


/*
main()
{
                int                     getMibFlag = 0;
                int                     firstTime = 0;
        mib2_tcp_t      tcp_struct;
                TCPStats                mTCPStats;


                mTCPStats.getMibData(&tcp_struct);

                // mTCPStats.printMTcpStats();

                mTCPStats.updataLast();

                // mTCPStats.printLastTcpStats();

                sleep(10);

                mTCPStats.getMibData(&tcp_struct);

                cout << "THE Last Stats!!" << endl;
                cout << endl;
                mTCPStats.printLastTcpStats();
                cout << endl;
                cout << endl;
                cout << "THE New stats!!" << endl;
                cout << endl;
                mTCPStats.printMTcpStats();
                cout << endl;
                cout << endl;
                mTCPStats.getDiffTcpStats();
                cout << "THE DIFFERENCES!!" << endl;
                cout << endl;
                mTCPStats.printDiffTcpStats();
                cout << "Finished!!" << endl;



}
*/
