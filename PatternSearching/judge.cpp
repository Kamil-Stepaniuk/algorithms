#include "judge.h"
#include <string.h>
#include <chrono> 
#include <stdio.h>
#include <math.h>
using namespace std::chrono; 
#define Name "s2"
#define PathIN "testCases/" Name ".in"

namespace {
char Text[1000000000];
char Phrase[1000000];
char Buffer[1000000];
long long seed;
int pseudoRand() {
	seed = seed * 13477581 + 1;
	return (seed & 0x7FFFFFFF) % 1000000007;
}
bool isMonolith(char *phrase) {
	int i = 1;
	while(phrase[i] != 0) {
		if( phrase[i-1] != phrase[i] )
			return false;
		i++;
	}
	return true;
}

}
void Judge::run(ISolution *solution) {
	freopen(PathIN, "r" , stdin);
	printf("%s\n", PathIN);
	int len, alphSize;
	scanf("%lld %d %d", &seed, &len, &alphSize);
	if(alphSize <= 1 || len <= 1) {
		printf("alphSize and len have to be above 1\n");
		return;
	}
	//--- generating phrase
	int i = 0;
	int phraseLen = (pseudoRand() % alphSize) + 2;
	while(i<phraseLen) {
		Phrase[i] = 'a' + pseudoRand() % alphSize;
		i++;
	}
	Phrase[phraseLen] = 0;
	if(isMonolith(Phrase)) {
		printf("monolith: %s \n", Phrase);
		Phrase[0] = 'a' + (('a' - Phrase[1] + 1) % alphSize);
		printf("modified: %s \n", Phrase);
	}
	i = log(len);
	while(i > 0) {
		int to = pseudoRand() % phraseLen;
		strncpy(Phrase + phraseLen, Phrase, to);
		phraseLen += to;
		Phrase[phraseLen] = 0;
		i--;
	}
	if(phraseLen > len) {
		phraseLen = len;
		Phrase[phraseLen] = 0;
	}
	//--- generating text
	i = 0;
	while(i < len) {
		strcpy(Text+i, Phrase);
		i += phraseLen - (((pseudoRand() % 6) * 6) % phraseLen);
	}
	Text[len] = 0;
	//--- running
	auto start = high_resolution_clock::now(); 
	int res = solution->count(Text, Phrase);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	printf("answer: %d, duration: %d\n", res, duration);
}
