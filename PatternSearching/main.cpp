#include <stdio.h>
#include <string.h>
#include "judge.h"

bool isSubString(char f[], char s[]) {
		int i = 0;
		while(f[i] != 0 && s[i] != 0 && f[i] == s[i])
			i++;
		return s[i] == 0;
	}

class NaiveSolution : public ISolution {
public:
	virtual int count(char text[], char phrase[]){
		int result = 0;
		int i = 0;
		while(text[i] != 0) {
			if(text[i] == phrase[0])
				if(isSubString(text+i+1, phrase+1))
					result++;
			i++;
		}
		return result;
	}
};

class RabinKarpSolution : public ISolution {
	int prime = 1000003;
	int base = 29;
	char idx(char s) {
		return s - 'a';
	}
	int hash( char s[], int len ) {
		long long p = 1;
		long long h = 0;
		int i = len - 1;
		while(i >= 0) {
			h = (h + p * idx(s[i])) % prime;
			p = (p * base) % prime;
			i--;
		}
		return h;
	}
public:
	virtual int count(char text[], char phrase[]) {
		int len = strlen(phrase);
		int pH = hash(phrase, len);
		int cH = hash(text, len);
		int p = 1;
		int i=0; while(i < len-1) p=p*base%prime, i++;
		int result = 0;
		int collision = 0;
		for(int i = 0; text[i+len-1] != 0; i++) {
			if(cH == pH)
				if(isSubString(text + i, phrase))
					result++;
				else
					collision++;
			cH = (base * (cH - idx(text[i]) * p) + idx(text[i+len])) % prime;
			cH = cH >= 0 ? cH : cH + prime;
		}
		//printf("collisions %d\n", collision);
		return result;
	}
};

class KMPSolution : public ISolution {
	int pi[100000];
	void fillFixes(int pi[], char p[]) {
		pi[0] = -1;
		int t = -1;
		for(int i=1; p[i-1] != 0; i++) {
			while(t >= 0 && p[t] != p[i-1]) //last prefix's letter and last in suffix's
				t = pi[t];
			t++;
			pi[i] = t;
		}
	}
public:
	virtual int count(char text[], char phrase[]) {
		int result = 0;
		fillFixes(pi, phrase);
		int i = 0
		  , j = 0;
		while( text[i] != 0 ) {
			while(phrase[j] != 0 && text[i+j] != 0 && phrase[j] == text[i+j])
				j++;
			if(phrase[j] == 0)
				result++;
			if(text[i + j] == 0)
				break;
			i = i + j - pi[j];
			j = pi[j] >= 0 ? pi[j] : 0;
		}
		return result;
	}
};

NaiveSolution naiveSolution;
RabinKarpSolution rabinKarpSolution;
KMPSolution kmpSolution;

int main() {
	printf("Naive solution\n");
	Judge::run(&naiveSolution);	
	printf("RabinKarp solution\n");
	Judge::run(&rabinKarpSolution);	
	printf("KMP solution\n");
	Judge::run(&kmpSolution);	
	return 0;
}
