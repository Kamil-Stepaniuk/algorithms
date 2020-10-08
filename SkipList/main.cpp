#include <stdio.h>
#include <chrono> 
using namespace std::chrono; 

#define LevelCount 5
#define MaxDivider 1 << (LevelCount-1)
#define VectorSize 10000

class Node;

struct Slice {
	Node* owner;
	Slice* next;
	int dist;
public:
	Slice()	{
		this->next = this;
		dist = 1;
	}
	void SetOwner(Node* owner){
		this->owner = owner;
	}
};

class Node{
public:
	Slice* slices;
	int value;
	int levels;
	Node(int value, int levels)
	{
		slices = new Slice[levels];
		for(int l = 0; l < levels; l++)
			slices[l].SetOwner(this);
		this->value = value;
		this->levels = levels;
	}
};

template<class T>
class Vector{
public:
	T* data = 0;
	int count = 0;
	int size = 0;
	void push(T value){
		if(count >= size) {
			int newSize = (size+1)*2;
			T* newData = new T[newSize];
			for(int i=0; i<count; i++)
				newData[i] = data[i];
			delete[] data;
			data = newData;
			size = newSize;
		}
		data[count] = value;
		count++;
	}
	int tryGet(T& value) {
		for(int i=0; i<count; i++)
			if(value.equals(data[i])) {
				value = data[i];
				return i;
			}
		return -1;
	}
	void remove(int idx) {
		if(idx < count){
			data[idx] = data[count-1];
			count--;
		}
	}
};

struct IdxNodePair {
	int idx;
	Node* node;
	bool equals(IdxNodePair& other) {
		return idx == other.idx;
	}
	int hash() {
		return idx % 1000000009;
	}
};

template<class T>
class HashMap {
	Vector<T> vector[VectorSize];
	int getIdx(T pair){
		return pair.hash() % VectorSize;
	}
public:
	void add(T pair) {
		vector[getIdx(pair)].push(pair);
	}
	bool tryGet(T& pair) {
		int idx = vector[getIdx(pair)].tryGet(pair);
		return idx >= 0;
	}
	void remove(T pair){
		int idx = vector[getIdx(pair)].tryGet(pair);
		if(idx >= 0)
			vector[getIdx(pair)].remove(idx);
	}
};

class SkipList { 
	struct Cursor{
		Slice* slice;
		int position;
	};
	int count=0;
	HashMap<IdxNodePair> map;
	Node sentinel = Node(-1, LevelCount);
	Cursor current, next;
	Cursor predecesors[LevelCount+1];
	int targetPosition;
	int Level = 0;
	int forLevel = 1;
	public:
	int drawLevel() {
		forLevel++;
		int divider = MaxDivider;
		int drawnLevel = LevelCount;
		while(forLevel % divider != 0)
			divider >>= 1, drawnLevel--;
		return drawnLevel;
	}
	bool canGoNext(){
		return current.slice->next->owner != &sentinel;
	}
	bool isNextEqualOrAfter() {
		return next.position >= targetPosition;
	}
	void goDown() {
		Level--;
		setCurrentAndNext({&current.slice->owner->slices[Level], current.position});
	}
	void goNext() {
		setCurrentAndNext(next);
	}
	void setCurrentAndNext(Cursor cursor) {
		current = cursor;
		next = {current.slice->next, current.position + current.slice->dist};
		predecesors[Level] = current;
	}
	bool canGoDown() {
		return Level > 0;
	}
	void fillPredecesors() {
		resetState();
		while(canGoDown()) {
			goDown();
			while(canGoNext() && isNextEqualOrAfter()==false)
				goNext();
		}
	}
	//	  -------prev dist----->
	// s1 ---d1---> s2 ---d2---> s3
	void insert(Node *n) {
		Level=0;
		while(Level < n->levels) {
			Slice* s1 = predecesors[Level].slice;
			Slice* s2 = &n->slices[Level];
			int d1 = targetPosition - predecesors[Level].position;
			int d2 = predecesors[Level].slice->dist - d1 + 1;
			link(s2, s1->next, d2);
			link(s1, s2, d1);
			Level++;
		}
		while(Level < LevelCount) {
			predecesors[Level].slice->dist++;
			Level++;
		}
	}
	void link(Slice* s1, Slice* s2, int dist) {
		s1->next = s2;
		s1->dist = dist;
	}
	void resetState() {
		Level = LevelCount;
		setCurrentAndNext({&sentinel.slices[LevelCount-1], -1});
	}
	void remove(Node *n) {
		Level=0;
		while(Level < n->levels) {
			unlink(
				predecesors[Level].slice,
				&n->slices[Level]);
			Level++;
		}
		while(Level < LevelCount) {
			predecesors[Level].slice->dist--;
			Level++;
		}
	}	
	//	  -------new dist------>
	// s1 ---d1---> s2 ---d2---> s3
	void unlink(Slice* s1, Slice* s2) {
		s1->dist += s2->dist - 1;
		s1->next = s2->next;
	}
	int getPosition(Node* n) {
		Level = n->levels-1;
		setCurrentAndNext({&n->slices[Level], 0});
		while(canGoNext()) {
			goNext();
			Level = current.slice->owner->levels-1;
		}
		return count - (current.position + current.slice->dist);
	}
public:
	void insert(int position, int value) {
		this->targetPosition = position;
		fillPredecesors();
		Node* n = new Node(value, drawLevel());
		map.add({value, n});
		insert(n);
		count++;
		
		int p = getPosition(n);
		if(position != p)
			printf("got %d actually is %d\n", p, position);
	}
	void remove(int position){
		this->targetPosition = position;
		fillPredecesors();
		Node* n = predecesors[0].slice->next->owner;
		if(n != &sentinel) {
			map.remove({n->value, n});
			remove(n);
			delete n->slices;
			delete n;
			count--;
		}
	}
	int removeByValue(int value){
		int result = 0;
		IdxNodePair pair = {value, 0};
		while(map.tryGet(pair)) {
			remove(getPosition(pair.node));
			result++;
		}
		return result;
	}
	void print(){
		setCurrentAndNext({&sentinel.slices[0], 0});
		while(canGoNext()){
			goNext();
			printf("%d ", current.slice->owner->value);
		}
		printf("\n");
	}
};

SkipList skipList;

void test1(){
	Vector<int> v;
	for(int i=0; i<10000; i++){
		skipList.insert(i, i);	
		v.push(i);
	}
	for(int i=10000; i>1; i--){
		skipList.remove(i-1);	
	}
	for(int i=10000; i>10; i--){
		v.remove(i-1);
	}
	for(int i=0; i<v.count; i++){
		printf("%d ", v.data[i]);
	}
	printf("\n");
}
void test2(){
	skipList.insert(0, 1);
	skipList.insert(1, 2);
	skipList.insert(2, 3);
	skipList.remove(1);
	skipList.insert(2, 4);
	skipList.insert(3, 5);
	skipList.remove(2);
	skipList.insert(3, 6);
	skipList.insert(4, 7);
}
void test3(){
	skipList.insert(0, 0);
	skipList.insert(1, 1);
	skipList.insert(2, 2);
	skipList.remove(2);
	skipList.remove(1);
}
void test4(){
	for(int i=0; i<10; i++){
		skipList.insert(i, i);	
	}
	for(int i=0; i<10; i++){
		skipList.insert(2*i, 10 + i);	
	}
	for(int i=0; i<4; i++){
		skipList.remove(i+2);	
	}
}
struct IdxPair {
	int idx;
	bool equals(IdxPair& other) {
		return idx == other.idx;
	}
	int hash() {
		return idx % 1000000009;
	}
};
void test5(){	
	HashMap<IdxPair> map;
	for(int i=0; i<100; i++){
		map.add({i*10000});
	}
	for(int i=0; i<10000*100; i++){
		IdxPair pair = {i};
		if(map.tryGet(pair)){
			printf("%d\n", i);
		}
		map.remove(pair);
	}
	printf("\n---\n");
	for(int i=0; i<10000*100; i++){
		IdxPair pair = {i};
		if(map.tryGet(pair)){
			printf("%d\n", i);
		}
		map.remove(pair);
	}
	printf("\n");
}
void test6(){
	skipList.insert(0, 1);
	skipList.insert(1, 2);
	skipList.insert(2, 3);
	skipList.removeByValue(2);
	skipList.insert(2, 4);
	skipList.insert(3, 5);
	skipList.removeByValue(5);
	skipList.insert(3, 6);
	skipList.insert(4, 7);
}
void test7(){
	skipList.insert(0, 1);
	skipList.insert(1, 2);
	skipList.insert(2, 1);
	printf("%d\n", skipList.removeByValue(1));
	skipList.insert(2, 2);
	skipList.insert(3, 2);
	printf("%d\n", skipList.removeByValue(1));
}

int main() {
	auto start = high_resolution_clock::now();
	//test1();
	//test2();
	//test3();
	//test4();
	//test5();
	//test6();
	test7();
	skipList.print();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	printf("duration: %d\n", duration);
	return 0;
}
