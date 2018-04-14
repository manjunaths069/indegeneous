#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stropts.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/sysinfo.h>
#include <kstat.h>
#include <inet/mib2.h>
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/socket.h>
#include <sys/tihdr.h>
#include <sys/tiuser.h>

#include <inet/led.h>
#include <inet/mib2.h>

#include <netinet/igmp_var.h>
#include <jni.h>


#define DEV_TCP "/dev/tcp"
#define DEV_IP "/dev/ip"

#define TCP_MODULE      "tcp"
#define IP_MODULE      "ip"

int errno;

//define attributes related micro
#define TcpActiveOpens 2
#define TcpPassiveOpens 3
#define TcpCurrEstab 4
#define TcpListenDrop 5
#define TcpHalfOpenDrop  6
#define TcpListenDropQ0 7
#define TcpInSegs  8
#define TcpInDataInorderSegs  9
#define TcpInDataUnorderSegs  10
#define TcpInDataDupSegs  11
#define TcpInDataPartDupSegs  12
#define TcpInBytes  13
#define TcpInDataInorderBytes  14
#define TcpInDataUnorderBytes  15
#define TcpInDataDupBytes  16
#define TcpInDataPartDupBytes  17
#define TcpOutSegs  18
#define TcpOutDataSegs 19
#define TcpRetransSegs 20
#define TcpOutBytes 21
#define TcpOutDataBytes 22
#define TcpRetransBytes 23
#define TcpAttemptFails 27
#define TcpOutRsts 28

class TCPStats
{
    private :

/* ********************************************* */
        float   mtcpActiveOpens;
        float   mtcpPassiveOpens;
                float   mtcpCurrEstab;
        float   mtcpListenDrop;
        float    mtcpHalfOpenDrop;
        float    mtcpListenDropQ0;
        float    mtcpInSegs;
        float    mtcpInDataInorderSegs;
        float    mtcpInDataUnorderSegs;
        float    mtcpInDataDupSegs;
        float    mtcpInDataPartDupSegs;
        float    mtcpInBytes;
        float    mtcpInDataInorderBytes;
        float    mtcpInDataUnorderBytes;
        float    mtcpInDataDupBytes;
        float    mtcpInDataPartDupBytes;
        float    mtcpOutSegs;
        float    mtcpOutDataSegs;
        float    mtcpRetransSegs;
        float    mtcpOutBytes;
        float    mtcpOutDataBytes;
        float    mtcpRetransBytes;
        float    mtcpAttemptFails;
        float    mtcpOutRsts;
/* ********************************************* */
        float    lasttcpActiveOpens;
        float    lasttcpPassiveOpens;
        float    lasttcpCurrEstab;
        float    lasttcpListenDrop;
        float    lasttcpHalfOpenDrop;
        float    lasttcpListenDropQ0;
        float    lasttcpInSegs;
        float    lasttcpInDataInorderSegs;
        float    lasttcpInDataUnorderSegs;
        float    lasttcpInDataDupSegs;
        float    lasttcpInDataPartDupSegs;
        float    lasttcpInBytes;
        float    lasttcpInDataInorderBytes;
        float    lasttcpInDataUnorderBytes;
        float    lasttcpInDataDupBytes;
        float    lasttcpInDataPartDupBytes;
        float    lasttcpOutSegs;
        float    lasttcpOutDataSegs;
        float    lasttcpRetransSegs;
        float    lasttcpOutBytes;
        float    lasttcpOutDataBytes;
        float    lasttcpRetransBytes;
        float    lasttcpAttemptFails;
        float    lasttcpOutRsts;
/* ********************************************* */
        float    difftcpActiveOpens;
        float    difftcpPassiveOpens;
        float    difftcpCurrEstab;
        float    difftcpListenDrop;
        float    difftcpHalfOpenDrop;
        float    difftcpListenDropQ0;
        float    difftcpInSegs;
        float    difftcpInDataInorderSegs;
        float    difftcpInDataUnorderSegs;
        float    difftcpInDataDupSegs;
        float    difftcpInDataPartDupSegs;
        float    difftcpInBytes;
        float    difftcpInDataInorderBytes;
        float    difftcpInDataUnorderBytes;
        float    difftcpInDataDupBytes;
        float    difftcpInDataPartDupBytes;
        float    difftcpOutSegs;
        float    difftcpOutDataSegs;
        float    difftcpRetransSegs;
        float    difftcpOutBytes;
        float    difftcpOutDataBytes;
        float    difftcpRetransBytes;
        float    difftcpAttemptFails;
        float    difftcpOutRsts;
/* ********************************************* */


    public :

                TCPStats();
                void    getMibData(mib2_tcp *tcp_struct, JNIEnv *, jobject );
                void    printMTcpStats();
                void    printLastTcpStats();
                void    printDiffTcpStats();
                void    getDifference();
                void    updataLast(JNIEnv *, jobject);
                void    getDiffTcpStats(JNIEnv *, jobject);
                void logDbg(JNIEnv *,jobject, char *);
                void logErr(JNIEnv *,jobject, char *);

        float    GetTcpActiveOpens() { return difftcpActiveOpens; };
        float    GetTcpPassiveOpens() { return difftcpPassiveOpens; };
        float    GetTcpCurrEstab() { return difftcpCurrEstab; };
        float    GetTcpListenDrop() { return difftcpListenDrop; };
        float    GetTcpHalfOpenDrop() { return difftcpHalfOpenDrop; };

        float    GetTcpListenDropQ0() { return difftcpListenDropQ0; };
        float    GetTcpInSegs() { return difftcpInSegs; };
        float    GetTcpInDataInorderSegs() { return difftcpInDataInorderSegs; };
        float    GetTcpInDataUnorderSegs() { return difftcpInDataUnorderSegs; };
        float    GetTcpInDataDupSegs() { return difftcpInDataDupSegs; };

        float    GetTcpInDataPartDupSegs() { return difftcpInDataPartDupSegs; };
        float    GetTcpInBytes() { return difftcpInBytes; };
        float    GetTcpInDataInorderBytes() { return difftcpInDataInorderBytes; };
        float    GetTcpInDataUnorderBytes() { return difftcpInDataUnorderBytes; };
        float    GetTcpInDataDupBytes() { return difftcpInDataDupBytes; };

        float    GetTcpInDataPartDupBytes() { return difftcpInDataPartDupBytes; };
        float    GetTcpOutSegs() { return difftcpOutSegs; };
        float    GetTcpOutDataSegs() { return difftcpOutDataSegs; };
        float    GetTcpRetransSegs() { return difftcpRetransSegs; };
        float    GetTcpOutBytes() { return difftcpOutBytes; };

        float    GetTcpOutDataBytes() { return difftcpOutDataBytes; };
        float    GetTcpRetransBytes() { return difftcpRetransBytes; };
        float    GetTcpAttemptFails() { return difftcpAttemptFails; };
        float    GetTcpOutRsts() { return difftcpOutRsts; };
};
