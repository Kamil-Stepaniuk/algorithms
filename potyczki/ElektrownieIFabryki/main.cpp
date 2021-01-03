#include <stdio.h>
#include <map>
#define x(y) //y
#define Max 500000
typedef long long int ll;
int N;
int Cities[Max + 1];
std::map<ll, int> Projects;

struct Project {
	ll power;
	int cost;
};

ll PowerZero = 0;
int CostZero = 0;

void push(ll power, int cost) {
	auto it = Projects.find(power);
	if ( it == Projects.end() || cost < (*it).second ) { 
		Projects[power] = cost;
		//removing with lower power and bigger cost
		auto it = Projects.find(power);
		while( it != Projects.begin() ) {
			auto it2 = it;
			--it2;
			if((*it2).second < cost )
				break;
			Projects.erase(it2);
		}
	}
}
bool tryGetMinimumCostWithPositivePower(int& cost) {
	auto it = Projects.lower_bound(-PowerZero);
	if(it != Projects.end()) {
		cost = (*it).second + CostZero;	
		return true;
	}
	return false;
}

void print() {
	printf("projects: \n");
	for( auto value : Projects ) 
		printf("(%lld  %d)\n", value.first, value.second);
	printf("\n");
}

void applyForAllProjects(int power) {
	PowerZero += power;
	CostZero++;
}

int main() {
	scanf("%d", &N);
	for(int i=0; i < N; i++)
		scanf("%d", &Cities[i]);
	push(Cities[0], 0);
	for(int i=1; i < N; i++) {
		x(printf("%d\n", Cities[i]));
		int p = Cities[i];
		x(print();)
		int minCost;
		if( tryGetMinimumCostWithPositivePower(minCost)) {
			applyForAllProjects(p);
			//try not use
			push(p - PowerZero, minCost - CostZero);
		}
		else
			applyForAllProjects(p);
	}
	int minCost;
	if( tryGetMinimumCostWithPositivePower(minCost))
		printf("%d\n", minCost);
	else
		printf("-1\n");	
	return 0;
}
