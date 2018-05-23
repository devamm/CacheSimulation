#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct cacheNode{
	int valid;
	long tag;
	struct cacheNode* next;
	//int block[256] = {0};
};

struct lol{
	int setA;
	long tagA;

	int setB;
	long tagB;

};
int ispow(int x);
int strTest(char* string);
int getlog2(int x);
void convert(char* string, int y, struct lol*, int n, int bSize);
long binToDec (char* bin);
void getbits(char* string, int n, int bSize, struct lol* rt);

int main(int argc, char** argv){
	if (argc != 5){
		printf("invalid number of inputs\n");
		exit(0);

	}
	
	int cacheSize = atoi (argv[1]); 
	int bSize = atoi (argv[3]);

	//make sure input is correctly formatted
	if (cacheSize <= 0 || bSize <= 0){
		printf("invalid input format\n");
		 exit(0);
	}

	//determines associativity of cache
	char* ascTest = argv[2];
	int asc = strTest(ascTest);
	//printf("%d\n",asc );
	
	// make sure cache size and block size are powers of 2
	if(ispow(cacheSize) == 0 || ispow(bSize) == 0){
		printf("cache size or block size incorrectly formatted\n");
		exit(0);
	}
	if(bSize > cacheSize){
		printf("invalid input\n");
		exit(0);
	}
	//all pieces of info collected and checked for validity, build cache
	// build cache here 
	int nNodes; //number of blocks in a set (number nodes in each hashtable entry)
	int n; //number of entries in hash table (#sets)
	if(asc == 0){
		//direct cache, #sets = cache size / block size
		n = cacheSize/bSize;
		nNodes = 1;
	} else if (asc == 1) {
		//fully associative
		n = 1;
		nNodes = cacheSize/bSize;
	} else {
		int deno = asc*bSize;
		n =  cacheSize/deno;
		nNodes = asc;
	}
	// build caches 
	struct cacheNode* cacheA[n];
	struct cacheNode* cacheB[n];
	int i;
	int x;
	//int tmpptr = 0;
	for(i = 0; i < n; i++){
		cacheA[i] = NULL;
		cacheB[i] = NULL; // makes sure there are no garbage pointers
		for (x = 0; x < nNodes; x++){
			struct cacheNode* aNode = (struct cacheNode*) malloc(sizeof(struct cacheNode));
			struct cacheNode* bNode = (struct cacheNode*) malloc(sizeof(struct cacheNode));
			aNode -> next = cacheA[i];
			bNode -> next = cacheB[i];
			aNode -> valid = 0;
			bNode -> valid = 0;
			aNode -> tag = 0L;
			bNode -> tag = 0L;
			cacheA[i] = aNode;
			cacheB[i] = bNode;
			//tmpptr++;
		}
	}

	//printf("number of nodes in each entry: %d\n",tmpptr);
	int readA = 0;
	int writeA = 0;
	int hitA = 0;
	int missA = 0;

	int readB = 0;
	int writeB = 0;
	int hitB = 0;
	int missB = 0;

	//scan file
	
	char* textfile = argv[4];
	FILE* fp;
	fp = fopen(textfile, "r");
	char addr[15];
	char pc[20];
	char rw;
	//int sc = 1;
	while(fscanf(fp, "%s  %c %s",pc, &rw, addr) != EOF){
		//printf("scan loop: %d\n",sc);
		//sc++;
		int test = strcmp(pc, "#eof");
		if (test == 0){
			break;
		}	
		int x = strlen(addr);
		int y = 14 - x;
		// temporary struct to hold set and index values derived from address
		struct lol* ad = (struct lol*) malloc(sizeof(struct lol));
		convert(addr, y, ad, n, bSize);
		
	
		if (rw == 'R'){
			//read from cache
			struct cacheNode* ptr = cacheA[ad->setA];
			struct cacheNode* ptrB = cacheB[ad->setB];
			int s = 0;
			while (ptr != NULL){
				if (ptr -> tag == ad -> tagA){
					hitA++; // cache hit
					s = 1;
					break;
				}
				ptr = ptr -> next;
			}
			//perform read on cache A 
			if (s == 0){
				// cache miss
				int e = 0;
				missA++;
				ptr = cacheA[ad->setA];
				while (ptr != NULL){
					if (ptr -> valid == 0){
						//read into next empty spot in cache 
						readA++;
						ptr -> tag = ad -> tagA;
						ptr -> valid = 1;
						e = 1;
						break;
					}
					ptr = ptr -> next;
				}
				if (e == 0){
					//read miss + no empty spots in cache (FIFO REPLACE)
					ptr = cacheA[ad->setA];
					if (nNodes == 1){
						ptr -> tag = ad->tagA;
					} else {
						int t;
						struct cacheNode* head = ptr;
						struct cacheNode* tail = ptr;
						for (t = 0; t < nNodes -1; t++){
							tail = tail -> next;
						}
						struct cacheNode* tmp = (struct cacheNode*) malloc(sizeof(struct cacheNode));
						tmp -> next = NULL;
						tmp -> valid = 1;
						tmp -> tag = ad -> tagA;
						tail -> next = tmp;
						cacheA[ad -> setA] = cacheA[ad -> setA] -> next;
						free(head);
					}
					readA++;
				}
			}
			s = 0;

			//perform read on cache B 
			while (ptrB != NULL){
				if (ptrB -> tag == ad -> tagB){
					hitB++; // hit 
					s = 1;
					break;
				}
				ptrB = ptrB -> next;
			}
			if (s == 0){
				// read miss 
				int e = 0;
				missB++;
				ptrB = cacheB[ad->setB];
				while (ptrB != NULL){
					if (ptrB -> valid == 0){
						//find next open spot in cache 
						readB++;
						ptrB -> tag = ad -> tagB;
						ptrB -> valid = 1;
						e = 1;
						break;
					}
					ptrB = ptrB -> next;
				}
				if (e == 0){
					//read miss + no empty spots in cache (FIFO replacement)
					ptrB = cacheB[ad->setB];
					if (nNodes == 1){
						ptrB -> tag = ad->tagB;
					} else {
						int t;
						struct cacheNode* head = ptrB;
						struct cacheNode* tail = ptrB;
						for (t = 0; t < nNodes -1; t++){
							tail = tail -> next;
						}
						struct cacheNode* tmp = (struct cacheNode*) malloc(sizeof(struct cacheNode));
						tmp -> next = NULL;
						tmp -> valid = 1;
						tmp -> tag = ad -> tagB;
						tail -> next = tmp;
						cacheB[ad -> setB] = cacheB[ad -> setB] -> next;
						free(head);
					}
					readB++;
				}
			}

		}
		if(rw == 'W'){
			//write from cache 
			struct cacheNode* ptr = cacheA[ad ->setA];
			struct cacheNode* ptrB = cacheB[ad -> setB];
			int s = 0;
			while (ptr != NULL){
				if (ptr -> tag == ad -> tagA){
					//write hit 
					hitA++;
					writeA++;
					s = 1;
					break;
				}
				ptr = ptr -> next;
			}
			if (s == 0){
				//write miss 
				int e = 0;
				missA++;
				ptr = cacheA[ad -> setA];
				while (ptr != NULL){
					if (ptr -> valid == 0){
						//write into next open spot
						ptr -> tag = ad -> tagA;
						ptr -> valid = 1;
						readA++;
						writeA++;
						e = 1;
						break;
					}
					ptr = ptr -> next;
				}
				if (e == 0){
					//no open spots available, FIFO replace 
					ptr = cacheA[ad->setA];
					if (nNodes == 1){
						ptr -> tag = ad->tagA;
					} else {
						int t;
						struct cacheNode* head = ptr;
						struct cacheNode* tail = ptr;
						for (t = 0; t < nNodes -1; t++){
							tail = tail -> next;
						}
						struct cacheNode* tmp = (struct cacheNode*) malloc(sizeof(struct cacheNode));
						tmp -> next = NULL;
						tmp -> valid = 1;
						tmp -> tag = ad -> tagA;
						tail -> next = tmp;
						cacheA[ad -> setA] = cacheA[ad -> setA] -> next;
						free(head);
					}
					readA++;
					writeA++;
				}
			}
			s = 0;
			//write for cacheB 
			while (ptrB != NULL){
				if (ptrB -> tag == ad -> tagB){
					hitB++;
					writeB++;
					s = 1;
					break;
				}
				ptrB = ptrB -> next;
			}
			if (s == 0){
				int e = 0;
				missB++;
				ptrB = cacheB[ad -> setB];
				while (ptrB != NULL){
					if (ptrB -> valid == 0){
						ptrB -> tag = ad -> tagB;
						ptrB -> valid = 1;
						readB++;
						writeB++;
						e = 1;
						break;
					}
					ptrB = ptrB -> next;
				}
				if (e == 0){
					ptrB = cacheB[ad->setB];
					if (nNodes == 1){
						ptrB -> tag = ad->tagB;
					} else {
						int t;
						struct cacheNode* head = ptrB;
						struct cacheNode* tail = ptrB;
						for (t = 0; t < nNodes -1; t++){
							tail = tail -> next;
						}
						struct cacheNode* tmp = (struct cacheNode*) malloc(sizeof(struct cacheNode));
						tmp -> next = NULL;
						tmp -> valid = 1;
						tmp -> tag = ad -> tagB;
						tail -> next = tmp;
						cacheB[ad -> setB] = cacheB[ad -> setB] -> next;
						free(head);
					}
					readB++;
					writeB++;
				}
			}
		}
		free(ad);
	} //end fscanf

	// print results
	printf("cache A\n");
	printf("Memory reads: %d\n",readA);
	printf("Memory writes: %d\n",writeA);
	printf("Cache hits: %d\n",hitA);
	printf("Cache misses: %d\n",missA);

	printf("cache B\n");
	printf("Memory reads: %d\n",readB);
	printf("Memory writes: %d\n",writeB);
	printf("Cache hits: %d\n",hitB);
	printf("Cache misses: %d\n",missB);

	return 0;
}

int ispow(int x){
	//determines if x is a power of 2 
	while((x % 2 == 0) && x > 1){
		x = x/2;
	}
	return (x == 1);
}

int strTest(char* string){
	//determines if cache is direct, fully associative or n-way associative from input 
	// returns associativity if n way, 0 if direct, 1 if fully associative 
	char dir[7];
	char ass[6];
	char assoc[100];

	strcpy(dir, "direct");
	strcpy(ass, "assoc");
	strcpy(assoc, "assoc:");

	if(strcmp(dir, string) == 0){ 	
		return 0;
		//direct cache
	}
	else if(strcmp(ass, string) == 0){
		return 1;
		//fully associative cache
	}
	else if(strncmp(string, assoc, 6) == 0){
		string += 6;
		int n = atoi(string);
		if (n <= 0){
			printf("invalid associativity\n");
			exit(0);
			//invalid number
		}
		return n;
	}
	else{
		//invalid input
		printf("invalid input format\n");
		exit(0);
	}
	printf("invalid input format\n");
	exit(0);
}

int getlog2(int x){
	//returns log2 of x
	double a = log10(x);
	double b = log10(2);
	double z = a/b;
	int n = (int) z;
	return n;
}

void convert(char* string, int y, struct lol* rt, int n, int bSize){
	//string is address in hex format 
	//converts hex string into binary string 
	// y is # of bits address needs to be extended by
	// store tag/set index obtained from address for cacheA and cacheB here 
	// n is number of sets 
	// bSize is block size in bits 

	string +=2;
	char address[13];
	strcpy(address, "");
	if (y == 0){
		//no zero extending required
		strcpy(address, string);
		//printf("No extending required, address: %s\n",address);
		
	} else {
		switch(y){
			case 1: strcat(address, "0"); strcat(address, string); break;
			case 2: strcat(address, "00"); strcat(address, string); break;
			case 3: strcat(address, "000"); strcat(address, string); break;
			case 4: strcat(address, "0000"); strcat(address, string); break;
			case 5: strcat(address, "00000"); strcat(address, string); break;
			case 6: strcat(address, "000000"); strcat(address, string); break;
			case 7: strcat(address, "0000000"); strcat(address, string); break;
			case 8: strcat(address, "00000000"); strcat(address, string); break;
			case 9: strcat(address, "000000000"); strcat(address, string); break;
			case 10: strcat(address,"0000000000"); strcat(address, string); break;
			case 11: strcat(address,"00000000000"); strcat(address, string); break;
			default: break;
		}
	}
	// address now zero extended 
	int x = strlen(address);
	x = x*4;
	x++;
	//printf("converting:%s\n",address);
	char bin[x];
	strcpy(bin, "");
	int i = 0;
	while(address[i]){
		switch(address[i]){
			case '0': strcat(bin, "0000"); break;
			case '1': strcat(bin, "0001"); break;
			case '2': strcat(bin, "0010"); break;
			case '3': strcat(bin, "0011"); break;
			case '4': strcat(bin, "0100"); break;
			case '5': strcat(bin, "0101"); break;
			case '6': strcat(bin, "0110"); break;
			case '7': strcat(bin, "0111"); break;
			case '8': strcat(bin, "1000"); break;
			case '9': strcat(bin, "1001"); break;
			case 'a': strcat(bin, "1010"); break;
			case 'b': strcat(bin, "1011"); break;
			case 'c': strcat(bin, "1100"); break;
			case 'd': strcat(bin, "1101"); break;
			case 'e': strcat(bin, "1110"); break;
			case 'f': strcat(bin, "1111"); break;
			default: break;
		}
		i++;
	}
	//address now in binary  (stored in array bin)
	getbits(bin,n,bSize, rt);

}

void getbits(char* string, int n, int bSize, struct lol* rt){
	//string contains entire 48bit address in binary
	//method gets tag and set values from address, stores values into struct lol 
	n = getlog2(n);
	bSize = getlog2(bSize);
	//printf("number of offset bits: %d\n",bSize );
	int tagSize = (48-n) - bSize;
	char tagAbits[tagSize+1];
	char tagBbits[tagSize+1];
	char setAbits[n+1];
	char setBbits[n+1];
	strcpy(tagAbits, "");
	strcpy(tagBbits, "");
	if (n > 1){
		strcpy(setAbits, "");
		strcpy(setBbits, "");	
	}

	//get tag for cacheA:
	int x;
	for (x = 0; x < tagSize; x++){
		tagAbits[x] = string[x];
	} 
	tagAbits[tagSize] = '\0';
	//get set index for cacheA
	int i;
	//x++;
	if (n > 0){
		for (i = 0; i < n; i++){
			setAbits[i] = string[x+i];
		}
		setAbits[n] = '\0';
	} else {
		rt -> setA = 0;
	}
	i = 0;
	x = 0;

	//get set for cacheB:
	if (n > 0){
		for (x = 0; x < n; x++){
			setBbits[x] = string[x];
		}
		setBbits[n] = '\0';
	} else {
		rt -> setB = 0;
	}
	//x++;
	//get tag for cacheB
	for (i = 0; i < tagSize; i++){
		tagBbits[i] = string[i+x];
	}
	tagBbits[tagSize] = '\0';
	

	rt -> tagA = binToDec(tagAbits);
	rt -> setA = (int) binToDec(setAbits);
	rt -> tagB = binToDec(tagBbits);
	rt -> setB = (int) binToDec(setBbits);
	//printf("original address: %s\n",string);
	//printf("index for A: %d tag for A: %ld\n",rt -> setA, rt ->tagA);
	//printf("index for B: %d tag for B: %ld\n",rt ->setB, rt ->tagB);

}

long binToDec (char* bin){
	//converts binary string into a long 
	return strtol(bin, NULL, 2);
}
