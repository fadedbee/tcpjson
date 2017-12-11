#define VER "00.00.01"
#define ROOT
#define CMDS_STOCK

#define STOCK_BNR 14
#define NUM_OPEN_FILES 5

//#define DEBUG

#include "../common/common.c"
#include <string.h>
#include <stdint.h>
#include "jsmn.h"
#include "niv_test.h" // contains the definition of STOCK

EXTERN RESRC *testPanel,*testDD;

void ffbackend_begin() {
 	sysHeaderDaemon();  
	printf("ffbackend starting...\r\n");
	
	testPanel = ffopenrsc("/home/multisys/source/resources/test.pnl");
	testDD = ffopenrsc("/home/multisys/source/resources/niv_test.rsc");	

	ffisam(0,SO,NUM_OPEN_FILES,512);
	int status = ffisam(STOCK_BNR,OF,testDD,"niv_test","",&STOCK,NULLP) && iserrno;
	if (status) {
                printf("ISAM Error\n");
		exit(1);
        }
}

void ffbackend_end() {
	ffisam(STOCK_BNR,CF);

	printf("ffbackend finished\r\n");
	ffend(0);
}

/*
 * zero all fields
 */
static void null_stock() {
	// TODO: Is there a quicker way?
        ASSL(STOCK.UNIQKEY, 0);
       	ASSS(STOCK.PCODE, "");
       	ASSS(STOCK.DESC, "");
       	ASSI(STOCK.BRAND_ID, 0);
       	ASSS(STOCK.MFG_P_NO, "");
       	ASSL(STOCK.LIST_PR, 0);
       	ASSL(STOCK.SELL_PR, 0);
       	ASSI(STOCK.UNIT_ID, 0);
        ASSS(STOCK.COLOUR, "");
	ASSS(STOCK.P_GROUP, "");
	ASSL(STOCK.LAST_PO, 0);
}

/*
 * This decodes the JSON in "body" as a stock item.
 * FIXME: It currently writes to a global STOCK struct, rather than taking a struct pointer as an argument.
 */
static int json_to_stock(char *body, size_t body_len) {
	printf("body: %.*s %d\n", body_len, body);

        // parse JSON
        jsmn_parser parser;
        jsmn_init(&parser);

        // parse once to find out how much space we need for tokens
        int num_tokens = jsmn_parse(&parser, body, body_len, NULL, 0);
        jsmntok_t t[num_tokens];

        // parse again to do the job
        // TODO: write a "next" interface for jsmn to return one token at a time, to avoid this double-parsing
        jsmn_init(&parser); // re-initialisation is essential, this
        num_tokens = jsmn_parse(&parser, body, body_len, t, num_tokens);
        if (num_tokens < 0) return -1; // TODO: improve error handling mechanism
        if (num_tokens < 1 || t[0].type != JSMN_OBJECT) return -2;


        for (int i = 1; i < num_tokens; i++) {
                if (jsmn_string_equal(body, &t[i], "uniqkey") == 0) { // TODO: Should UNIQKEY ever be set?
        		ASSL(STOCK.UNIQKEY,atol(body + t[++i].start));
                } else if (jsmn_string_equal(body, &t[i], "pcode") == 0) {
			++i;
		  	size_t pcode_len = t[i].end - t[i].start;
			char pcode_zts[pcode_len + 1];
			memcpy(pcode_zts, body + t[i].start, pcode_len);
			pcode_zts[pcode_len] = '\0';
        		ASSS(STOCK.PCODE, pcode_zts); // FIXME: Find a way of assigning without having to create zero terminated strings.
                } else if (jsmn_string_equal(body, &t[i], "desc") == 0) {
			++i;
		  	size_t desc_len = t[i].end - t[i].start;
			char desc_zts[desc_len + 1];
			memcpy(desc_zts, body + t[i].start, desc_len);
			desc_zts[desc_len] = '\0';
        		ASSS(STOCK.DESC, desc_zts);
                } else if (jsmn_string_equal(body, &t[i], "brand_id") == 0) {
        		ASSI(STOCK.BRAND_ID,atoi(body + t[++i].start));
                } else if (jsmn_string_equal(body, &t[i], "mfg_p_no") == 0) {
			++i;
		  	size_t mfg_p_no_len = t[i].end - t[i].start;
			char mfg_p_no_zts[mfg_p_no_len + 1];
			memcpy(mfg_p_no_zts, body + t[i].start, mfg_p_no_len);
			mfg_p_no_zts[mfg_p_no_len] = '\0';
        		ASSS(STOCK.MFG_P_NO, mfg_p_no_zts);
                } else if (jsmn_string_equal(body, &t[i], "list_pr") == 0) {
        		ASSL(STOCK.LIST_PR,atol(body + t[++i].start));
                } else if (jsmn_string_equal(body, &t[i], "sell_pr") == 0) {
        		ASSL(STOCK.SELL_PR,atol(body + t[++i].start));
                } else if (jsmn_string_equal(body, &t[i], "unit_id") == 0) {
        		ASSI(STOCK.UNIT_ID,atoi(body + t[++i].start));
                } else if (jsmn_string_equal(body, &t[i], "in_stk") == 0) {
        		ASSL(STOCK.IN_STK,atol(body + t[++i].start));
                } else if (jsmn_string_equal(body, &t[i], "colour") == 0) {
			++i;
		  	size_t colour_len = t[i].end - t[i].start;
			char colour_zts[colour_len + 1];
			memcpy(colour_zts, body + t[i].start, colour_len);
			colour_zts[colour_len] = '\0';
        		ASSS(STOCK.COLOUR, colour_zts);
                } else if (jsmn_string_equal(body, &t[i], "p_group") == 0) {
			++i;
		  	size_t p_group_len = t[i].end - t[i].start;
			char p_group_zts[p_group_len + 1];
			memcpy(p_group_zts, body + t[i].start, p_group_len);
			p_group_zts[p_group_len] = '\0';
        		ASSS(STOCK.P_GROUP, p_group_zts);
                } else if (jsmn_string_equal(body, &t[i], "last_po") == 0) {
        		ASSL(STOCK.LAST_PO,atol(body + t[++i].start));
                } else {
                        printf("Unexpected Key: |%.*s|\n", t[i].end-t[i].start, body + t[i].start);
                }
        }
	return 0;
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
size_t ffbackend_put_stock(char *pcode, size_t pcode_len, char *body, size_t body_len, char *out, size_t out_len) {
  	null_stock();
	int r = json_to_stock(body, body_len);
	if (r >= 0) {
        	if (r = ffisam(STOCK_BNR,IR)) {
	  		printf("ISAM error writing stock: %d\n", r);
		} else {
			return snprintf(out, out_len, "{\"code\":201, \"body\":\"created\"}\n");
		}
	} else {
	  	printf("error json_to_stock: %d\n", r);
	}
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

        if (!ffisam(STOCK_BNR,RK,1,0)) {
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
        char pcode_zts[pcode_len + 1];
        memcpy(pcode_zts, pcode, pcode_len);
        pcode_zts[pcode_len] = '\0';
        ASSS(STOCK.PCODE, pcode_zts); // FIXME: Find a way of assigning without having to create zero terminated strings.

        if (!ffisam(STOCK_BNR,RK,1,0)) {
		int r = json_to_stock(body, body_len);
		if (r >= 0) {
        		if (!ffisam(STOCK_BNR,WC)) {
				return snprintf(out, out_len, "{\"code\":200, \"body\":\"updated\"}\n");
			} else {
				return snprintf(out, out_len, "{\"code\":500, \"body\":\"ISAM error\"}\n");
			}
		} else {
			return snprintf(out, out_len, "{\"code\":500, \"body\":\"JSON error\"}\n");
		}
	} else {
		return snprintf(out, out_len, "{\"code\":404, \"body\":\"stock pcode not found\"}\n");
	}
}

/*
 * Delete a stock item, or output an error.
 */
size_t ffbackend_delete_stock(char *pcode, size_t pcode_len, char *out, size_t out_len) {
	null_stock(); // blank the stock record

        char pcode_zts[pcode_len + 1];
        memcpy(pcode_zts, pcode, pcode_len);
        pcode_zts[pcode_len] = '\0';
        ASSS(STOCK.PCODE, pcode_zts); // FIXME: Find a way of assigning without having to create zero terminated strings.

        if (!ffisam(STOCK_BNR,RK,1,0)) {
        	if (!ffisam(STOCK_BNR,DC,1,0)) {
			return snprintf(out, out_len, "{\"code\":200, \"body\":\"deleted\"}\n");
		} else {
			return snprintf(out, out_len, "{\"code\":500, \"body\":\"ISAM error\"}\n");
		}
	} else {
		return snprintf(out, out_len, "{\"code\":404, \"body\":\"stock pcode not found\"}\n");
	}
}

