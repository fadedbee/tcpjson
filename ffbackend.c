#define VER "00.00.01"
#define ROOT
#define CMDS_STOCK

#define STOCK_BNR 14
#define NUM_OPEN_FILES 5

//#define DEBUG

#include "../common/common.c"
#include <string.h>

#include "niv_test.h" // contains the definition of STOCK

EXTERN RESRC *testPanel,*testDD;

int ffbackend()
{
 	sysHeaderDaemon();  
	printf("ffbackend starting...\r\n");
	
	testPanel = ffopenrsc("/home/multisys/source/resources/test.pnl");
	testDD = ffopenrsc("/home/multisys/source/resources/niv_test.rsc");	

        printf("pre-ISAM C\n");
	ffisam(0,SO,NUM_OPEN_FILES,512);
	int status = ffisam(STOCK_BNR,OF,testDD,"niv_test","",&STOCK,NULLP) && iserrno;
	if (status) {
                printf("ISAM Error C\n");
		exit(1);
        }
	

/*
        printf("pre-ISAM A\n");
        ASSS(STOCK.PCODE,"1234");
        ASSS(STOCK.DESC, "TEST DATA2");
        ASSI(STOCK.BRAND_ID,1);
        ASSS(STOCK.MFG_P_NO,"TEST_MFG_PART_NO2");
        ASSL(STOCK.LIST_PR,2.00L);
        ASSL(STOCK.SELL_PR,3.00L);
        ASSI(STOCK.UNIT_ID,1);
        ASSS(STOCK.COLOUR,"YLW");
        ASSS(STOCK.P_GROUP,"TEST");
        ASSL(STOCK.LAST_PO,291117);

        if(ffisam(STOCK_BNR,IR)) {
                printf("ISAM Error A\n");
		exit(1);
        }
*/


        printf("pre-ISAM B\n");
        ASSS(STOCK.PCODE,"1234");
        printf("pre-ISAM B1\n");
        if(!ffisam(STOCK_BNR,RK,1,0)) {
                printf("post-ISAM B2\n");
                char description[sizeof(STOCK.DESC)+1];
                GETSTR(description,STOCK.DESC);
                printf("post-ISAM B3\n");
                printf("%s\n",description);
        } else {
                printf("ISAM Error B\n");
		exit(1);
        }
        printf("post-ISAM B4\n");

	ffisam(STOCK_BNR,CF);

	printf("ffbackend finishing...\r\n");
	sleep(3);
	ffend(0);
}
