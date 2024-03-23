#include <iostream>
#include <vector>
#include <thread>
#include <fstream>

using namespace std;

ofstream out1("Output1.txt");
ofstream out2("Output2.txt");

class Timer {
public:
	chrono::system_clock::time_point Begin;
	chrono::system_clock::time_point End;
	chrono::system_clock::duration RunTime;
	Timer() {//constructor
		Begin = chrono::system_clock::now();
	}
	~Timer() {
		End = chrono::system_clock::now();
		RunTime = End - Begin;
		cout << "Run Time is " << chrono::duration_cast<chrono::milliseconds>(RunTime).count() << "ms" << endl;
	}
};

void RecurSort(vector<int>& Data, int n1, int n2, bool Up);
void RecurFront(vector<int>& Data, int n1, int n2, bool up);

void HalfUp(vector<int>& Data, int n1, int n2, int n);
void HalfDown(vector<int>& Data, int n1, int n2, int n);
void QuarterUp(vector<int>& Data, int n1, int n2, int n);

int main() {
	
	int n{ 1024 }, m{ 1000 };
	vector<int> Data(n);
	for (auto& i : Data) i = rand() % m;


	{
		Timer TT;
		RecurSort(Data, 0, n-1, true);
	}

	for (int i = 0; i < Data.size(); ++i) {
		out1 << Data[i] << " ";
		if (i % 20 == 0) out1 << endl;
	}
	cout << endl;
	cout << "##################" << endl;

	for (auto& i : Data) i = rand() % m;

	{
		Timer T;
		{
			RecurSort(Data, 0, (n - 1) / 4, true);
			thread t1(RecurSort, ref(Data), (n - 1) / 4 + 1, (n - 1) / 2, false);
			thread t2(RecurSort, ref(Data), (n - 1) / 2 + 1, 3 * (n - 1) / 4, true);
			thread t3(RecurSort, ref(Data), 3 * (n - 1) / 4 + 1, (n - 1), false);
			t1.join();
			t2.join();
			t3.join();
		}
		
		//second last step
		{
			HalfUp(Data, 0, n / 8 - 1, n);
			thread t1(HalfUp, ref(Data), n / 8, n / 4 - 1, n);
			thread t2(HalfDown, ref(Data), n / 2, (n / 2) + (n / 8 - 1), n);
			thread t3(HalfDown, ref(Data), (n / 2) + (n / 8), (n / 2) + (n / 8) + (n / 8 - 1), n);
			t1.join();
			t2.join();
			t3.join();
		}
		{
			RecurFront(Data, 0, (n - 1) / 4, true);
			thread t1(RecurFront, ref(Data), (n - 1) / 4 + 1, (n - 1) / 2, true);
			thread t2(RecurFront, ref(Data), (n - 1) / 2 + 1, 3 * (n - 1) / 4, false);
			thread t3(RecurFront, ref(Data), 3 * (n - 1) / 4 + 1, (n - 1), false);
			t1.join();
			t2.join();
			t3.join();
		}
		
		//last step
		{
			QuarterUp(Data, 0, n / 8 - 1, n);
			thread t1(QuarterUp, ref(Data), n / 8, n / 4 - 1, n);
			thread t2(QuarterUp, ref(Data), n / 4, n / 4 + (n / 8 - 1), n);
			thread t3(QuarterUp, ref(Data), (n / 4) + (n / 8), n / 2 - 1, n);
			t1.join();
			t2.join();
			t3.join();
		}
		{
			HalfUp(Data, 0, n / 8 - 1, n);
			thread t1(HalfUp, ref(Data), n / 8, n / 4 - 1, n);
			thread t2(HalfUp, ref(Data), n / 2, (n / 2) + (n / 8 - 1), n);
			thread t3(HalfUp, ref(Data), (n / 2) + (n / 8), (n / 2) + (n / 8) + (n / 8 - 1), n);
			t1.join();
			t2.join();
			t3.join();
		}
		{
			RecurFront(Data, 0, (n - 1) / 4, true);
			thread t1(RecurFront, ref(Data), (n - 1) / 4 + 1, (n - 1) / 2, true);
			thread t2(RecurFront, ref(Data), (n - 1) / 2 + 1, 3 * (n - 1) / 4, true);
			thread t3(RecurFront, ref(Data), 3 * (n - 1) / 4 + 1, (n - 1), true);
			t1.join();
			t2.join();
			t3.join();
		}
		
	}
	for (int i = 0; i < Data.size(); ++i) {
		out2 << Data[i] << " ";
		if (i % 20 == 0) out2 << endl;
	}

	return 0;
}

void RecurFront(vector<int>& Data, int n1, int n2, bool up) {
	for (int i = 0; i <= (n2 - n1) / 2; ++i) {
		if (up) {
			if (Data[n1 + i] > Data[n1 + i + (n2 - n1 + 1) / 2]) swap(Data[n1 + i], Data[n1 + i + (n2 - n1 + 1) / 2]);
		}
		else if (Data[n1 + i] < Data[n1 + i + (n2 - n1 + 1) / 2]) swap(Data[n1 + i], Data[n1 + i + (n2 - n1 + 1) / 2]);
	}
	if (n2 - n1 == 1) return;
	RecurFront(Data, n1, (n2 + n1) / 2, up);
	RecurFront(Data, (n2 + n1) / 2 + 1, n2, up);
}

void RecurSort(vector<int>& Data, int n1, int n2, bool Up) {
	if (n2 - n1 == 1) {
		if (Up) {
			if (Data[n1] > Data[n2]) swap(Data[n1], Data[n2]);
		}
		else {
			if (Data[n1] < Data[n2]) swap(Data[n1], Data[n2]);
		}
		return;
	}
	RecurSort(Data, n1, (n1 + n2) / 2, true);
	RecurSort(Data, (n1 + n2) / 2 + 1, n2, false);
	RecurFront(Data, n1, n2, Up);
}

void HalfUp(vector<int>& Data, int n1, int n2, int n) {
	for (int i = n1;i <= n2;i++) {
		if (Data[i] > Data[i + (n / 4)]) {
			swap(Data[i], Data[i + (n / 4)]);
		}
	}
}
void HalfDown(vector<int>& Data, int n1, int n2, int n) {
	for (int i = n1;i <= n2;i++) {
		if (Data[i] < Data[i + (n / 4)]) {
			swap(Data[i], Data[i + (n / 4)]);
		}
	}
}
void QuarterUp(vector<int>& Data, int n1, int n2, int n) {
	for (int i = n1;i <= n2;i++) {
		if (Data[i] > Data[i + (n / 2)]) {
			swap(Data[i], Data[i + (n / 2)]);
		}
	}
}
