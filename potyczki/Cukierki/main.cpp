#include <stdio.h>
#define Max 5000
int N;
#define Moudlo 1000000007
typedef long long int ll;

int NotSorted[Max+2];
int Sorted[Max+2];
int Counts[Max+2]; //[i] contains number of combinations giving sum i
int counters[Max+2];

void csort(int* from, int* to, int size, int max) {
	for(int i = 0; i < size; i++)
		counters[from[i]]++;
	for(int i = 0, j = 0; i <= max; i++)
		while (counters[i])
			counters[i]--, to[j] = i, j++;
}

int main() {
	scanf("%d", &N);
	for(int i=0; i < N; i++)
		scanf("%d", &NotSorted[i]);
	csort(NotSorted, Sorted, N, Max);
	Counts[0] = 1;
	ll aboveSum = 0;
	for(int n = 0; n < N; n++) {
		int pack = Sorted[n];
		aboveSum = (aboveSum + aboveSum) % Moudlo;
		for(int i = Max; i >= pack - 1; i--) {
			int target = i + pack;
			if(target > Max)
				aboveSum = (aboveSum + Counts[i]) % Moudlo;
			else
				Counts[target] = (Counts[target] + Counts[i]) % Moudlo;
		}
	}
	ll sum = aboveSum;
	for(int i = Max; i >= 1; i--)
		sum = (sum + Counts[i]) % Moudlo;
	printf("%lld", sum);
	return 0;
}
