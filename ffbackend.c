#define VER "00.00.01"
#define ROOT
#define CMDS_STOCK

#define STOCK_BNR 14
#define NUM_OPEN_FILES 5

//#define DEBUG

#include "../common/common.c"
#include <string.h>
#include <stdint.h>

#include "niv_test.h" // contains the definition of STOCK

EXTERN RESRC *testPanel,*testDD;



void ffbackend_begin() {
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

*/

void ffbackend_end() {
	ffisam(STOCK_BNR,CF);

	printf("ffbackend finishing...\r\n");
	sleep(3);
	ffend(0);
}

/*
 * This decodes the JSON in "body" as a stock item.
 * FIXME: It currently writes to a global STOCK struct, rather than taking a struct as an argument.
 */
static size_t json_to_stock(char *body, size_t body_len) {
}

/*
 * This snprintfs to the out buffer.
 * If the return value is greater-than or equal to out_len, then the buffer was not big enough.
 * FIXME: It currently reads from a global STOCK struct, rather than taking a struct as an argument.
 */
static size_t stock_to_json(char* out, size_t out_len) {
	// FIXME: The field names and types should be inferred at runtime from the data dictionary,
	// rather than being hard-coded.
	// There will need to be overrides for some names and conversion factors for some fields.
        int32_t uniqkey;
        char pcode[sizeof(STOCK.PCODE)+1];
        char desc[sizeof(STOCK.DESC)+1];
        int16_t brand_id;
        char mfg_p_no[sizeof(STOCK.MFG_P_NO)+1];
        int32_t list_pr;
        int32_t sell_pr;
        int16_t unit_id;
        int32_t in_stk;
        char colour[sizeof(STOCK.COLOUR)+1];
        char p_group[sizeof(STOCK.P_GROUP)+1];
        int32_t last_po; //date
        uniqkey = GETL(STOCK.UNIQKEY);
        GETSTR(pcode,STOCK.PCODE);
        GETSTR(desc,STOCK.DESC);
        brand_id = GETI(STOCK.BRAND_ID); // FIXME: how do I get a short?
        GETSTR(mfg_p_no,STOCK.MFG_P_NO);
        list_pr = GETL(STOCK.LIST_PR);
        sell_pr = GETL(STOCK.SELL_PR);
        unit_id = GETI(STOCK.UNIT_ID); // FIXME: how do I get a short?
        in_stk = GETL(STOCK.IN_STK);
        GETSTR(colour,STOCK.COLOUR);
        GETSTR(p_group,STOCK.P_GROUP);
        last_po = GETL(STOCK.LAST_PO); // FIXME: how do dates work in Multisys/FRED?
	// TODO: This function would be better if it just returned the "body" section below and another 
	// function (at a higher layer) wrapped it with the code.
	return snprintf(out, out_len, "{\"code\": 200, \"body\":{"
			"\"uniqkey\":%d, "
			"\"pcode\":\"%s\", "
			"\"desc\":\"%s\", "
			"\"brand_id\":%d, "
			"\"mfg_p_no\":\"%s\", "
			"\"list_pr\":%d, "
			"\"sell_pr\":%d, "
			"\"unit_id\":%d, "
			"\"in_stk\":%d, "
			"\"colour\":\"%s\", "
			"\"p_group\":\"%s\", "
			"\"last_po\":%d"
			"}}\n", 
			uniqkey, pcode, desc, brand_id, mfg_p_no, list_pr, sell_pr, unit_id,
			in_stk, colour, p_group, last_po);
}

/*
 * Create a new stock item, or output an error.
 */
size_t ffbackend_put_stock(char *pcode, size_t pcode_len, char *body, char body_len, char *out, size_t out_len) {
	return snprintf(out, out_len, "{\"code\":500, \"body\":\"error creating stock item\"}\n");
}

/*
 * Get a stock item, or output an error.
 * FIXME: For PoC these functions do the JSON formatting, as well as the data retreival.
 * TODO: Add a separate serialisation layer.
 */
size_t ffbackend_get_stock(char *pcode, size_t pcode_len, char *out, size_t out_len) {
	char pcode_zts[pcode_len + 1];
	memcpy(pcode_zts, pcode, pcode_len);
	pcode_zts[pcode_len] = '\0';
        ASSS(STOCK.PCODE, pcode_zts); // FIXME: Find a way of assigning without having to create zero terminated strings.

        if(!ffisam(STOCK_BNR,RK,1,0)) {
		size_t num = stock_to_json(out, out_len);
		if (num >= out_len) {
			return snprintf(out, out_len, "{\"code\":500, \"body\":\"JSON longer than output buffer\"}\n");
		} else {
			return num;
		}
        } else {
		return snprintf(out, out_len, "{\"code\":404, \"body\":\"stock pcode not found\"}\n");
        }
}

/*
 * Update a stock item, or output an error.
 */
size_t ffbackend_post_stock(char *pcode, size_t pcode_len, char *body, char body_len, char *out, size_t out_len) {
}

/*
 * Delete a stock item, or output an error.
 */
size_t ffbackend_delete_stock(char *pcode, size_t pcode_len, char *out, size_t out_len) {
}

