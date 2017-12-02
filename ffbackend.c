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
	sys_header("niv_test","niv_test");
	printf("ffbackend starting...\r\n");
	
	testPanel = ffopenrsc("/home/multisys/source/resources/test.pnl");
	testDD = ffopenrsc("/home/multisys/source/resources/niv_test.rsc");	

	ffisam(0,SO,NUM_OPEN_FILES,512);
	int status = ffisam(STOCK_BNR,OF,testDD,"niv_test","",&STOCK,NULLP) && iserrno;
	if (status) exit(1);
	
	ffisam(STOCK_BNR,CF);

	printf("ffbackend finishing...\r\n");
	sleep(3);
	ffend(0);
}
