# TCPJSON
A proof-of-concept JSON/TCP server.

## Principle Source Files:

- tcpjson.c   - This is the parent process which owns the server socket and calls `fork` to start child processes for each TCP connection.
- handle.  c  - Each child process calls handle() which processes TCP connection input line-by-line (of JSON).
- ffbackend.c - The only part of the system which is FRED/ISAM aware.  It receives JSON and stock codes, and returns JSON strings.

## Other Source Files

- readline.c  - An inefficent, naive, line splitter.
- jsmn.c      - A zero-copy JSON parser.  https://github.com/zserge/jsmn

## Protocol

The protocol used on top of TCP is Newline Delimited JSON.  i.e. lines of JSON, one JSON object per line.
This JSON protocol is structured so as to be easy to convert to an HTTP REST interface, if required, later.

## -Building and Running

Log in as mspd.

<pre>
$ cd source/tcpjson
$ make clean
$ make
$ ./tcpjson
</pre>

## Testing

In another log-in session:

<pre>
$ cp orig.niv_test.dat niv_test.dat
$ cp orig.niv_test.idx niv_test.idx
$ ./test.expect
</pre>

N.B. Resetting the data files is only required so that the created record has the expected UNIQ_KEY of 3.

## Results of Test
<pre>
tcpjson started...
child
parent
ffbackend starting...
rx: {"method":"GET", "path":["ping"]}
tx: {"code": 204}

rx: {"method":"GET", "path":["ping"]}
tx: {"code": 204}

rx: {"method":"GET", "path":["a","path","that","is","not","handled"]}
unhandled json: {"method":"GET", "path":["a","path","that","is","not","handled"]}
tx: {"code": 404, "body":"unhandled path"}

rx: {"method":"GET", "path":["stock","BADCODE"]}
tx: {"code":404, "body":"stock pcode not found"}

rx: {"method":"PUT", "path":["stock", "5678"], "body":{"pcode":"5678", "desc":"2", "brand_id":1, "mfg_p_no":"TEST_MFG_PART_NO2", "list_pr":2, "sell_pr":3, "unit_id":1, "in_stk":0, "colour":"YLW", "p_group":"TEST", "last_po":291117}}
body: {"pcode":"5678", "desc":"2", "brand_id":1, "mfg_p_no":"TEST_MFG_PART_NO2", "list_pr":2, "sell_pr":3, "unit_id":1, "in_stk":0, "colour":"YLW", "p_group":"TEST", "last_po":291117} 0
tx: {"code":201, "body":"created"}

rx: {"method":"GET", "path":["stock", "5678"]}
tx: {"code": 200, "body":{"uniqkey":3, "pcode":"5678", "desc":"2", "brand_id":1, "mfg_p_no":"TEST_MFG_PART_NO2", "list_pr":2, "sell_pr":3, "unit_id":1, "in_stk":0, "colour":"YLW", "p_group":"TEST", "last_po":291117}}

rx: {"method":"POST", "path":["stock", "5678"], "body":{"mfg_p_no":"NEW_PART_NO", "colour":"GRN"}}
body: {"mfg_p_no":"NEW_PART_NO", "colour":"GRN"} 3
tx: {"code":200, "body":"updated"}

rx: {"method":"GET", "path":["stock", "5678"]}
tx: {"code": 200, "body":{"uniqkey":3, "pcode":"5678", "desc":"2", "brand_id":1, "mfg_p_no":"NEW_PART_NO", "list_pr":2, "sell_pr":3, "unit_id":1, "in_stk":0, "colour":"GRN", "p_group":"TEST", "last_po":291117}}

rx: {"method":"DELETE", "path":["stock", "5678"]}
tx: {"code":200, "body":"deleted"}

rx: {"method":"GET", "path":["stock","5678"]}
tx: {"code":404, "body":"stock pcode not found"}

rx: {"method":"DELETE", "path":["stock", "5678"]}
tx: {"code":404, "body":"stock pcode not found"}

rx: 
handling completed
ffbackend finished
</pre>

## Further Work

- Is jsmn the best way to parse JSON?  I've previously used http://www.digip.org/jansson/ on other projects.
- Conversion between JSON request bodies and ISAM buffers needs to be generated as part of the build.  We cannot write conversion functions for every record type.
- Handling-functions will also need to be generated automatically.
- Error handling could be improved.
- Locking should be implemented on the POST and DELETE methods.
- Add an example of selecting multiple records by some form of search.
- See all the FIXME: and TODO: notes in the source.

