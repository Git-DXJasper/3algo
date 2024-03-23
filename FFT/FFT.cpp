////HW2_Xie_Dongze

#include <iostream>
#include <complex>
#include <math.h>
#include <vector>
#include <fstream>
#include <thread>

using namespace std;
typedef complex<double> cd;

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


const cd J{ 0,1 };
const double PI = 3.1415926536;

unsigned int bitReverse(unsigned int x, int N) {
	unsigned int res{ 0 };
	for (int i = log2(N) - 1;i >= 0;i--) {
		res += x % 2 << i;
		x /= 2;
	}
	return res;
}

cd W(int N, int r) {
	if (r == 0) return 1;
	return exp(cd(-2) * cd(PI) * J * cd(r) / cd(N));
}

void recur_fft(vector<cd>& a, vector<cd>& b, int start, int end, int N) {
	if (end - start != 1) {
		recur_fft(a, b, start, (start + end) / 2, N);
		recur_fft(a, b, (start + end + 1) / 2, end, N);
		for (int i = start;i <= end;i++) {
			a[i] = b[i];
		}
	}
	int stage{ int(log2(end + 1 - start)) };

	//up lines,+W
	for (int i = 0;i < pow(2, stage - 1);i++) {
		b[start + i] = a[start + i] + W(pow(2, stage), i) * a[start + i + pow(2, stage - 1)];
	}

	//down lines,-W
	for (int i = 0;i < pow(2, stage - 1);i++) {
		b[start + i + pow(2, stage - 1)] = a[start + i] - W(pow(2, stage), i) * a[start + i + pow(2, stage - 1)];
	}
}


void iter_dft(vector<cd>& a, vector<cd>& b, int stage, int part, int N) {
	int begin{ int(pow(2, stage) * (part - 1)) };
	//up lines,+W
	for (int i = 0;i < pow(2, stage - 1);i++) {
		b[begin + i] = a[begin + i] + W(pow(2, stage), i) * a[begin + i + pow(2, stage - 1)];
	}

	//down lines,-W
	for (int i = 0;i < pow(2, stage - 1);i++) {
		b[begin + i + pow(2, stage - 1)] = a[begin + i] - W(pow(2, stage), i) * a[begin + i + pow(2, stage - 1)];
	}
}

void iter_fft(vector<cd>& a, vector<cd>& b, int N) {
	for (int stage = 1;stage <= log2(N);stage++) {
		for (int part = 1;part <= N / pow(2, stage);part++) {
			iter_dft(a, b, stage, part, N);
		}
		for (int i = 0;i < a.size();i++) {
			a[i] = b[i];
		}
	}
}

void iter_fft_thread(vector<cd>& a, vector<cd>& b, int N, int stage, int partl, int partr) {
	for (int part = partl;part <= partr;part++) {
		iter_dft(a, b, stage, part, N);
	}
}

void upLines(vector<cd>& a, vector<cd>& b, int begin) {
	int stage{ int(log2(a.size()) - 1) };
	for (int i = 0;i < pow(2, stage - 1);i++) {
		b[begin + i] = a[begin + i] + W(pow(2, stage), i) * a[begin + i + pow(2, stage - 1)];
	}
}

void downLines(vector<cd>& a, vector<cd>& b, int begin) {
	int stage{ int(log2(a.size()) - 1) };
	for (int i = 0;i < pow(2, stage - 1);i++) {
		b[begin + i + pow(2, stage - 1)] = a[begin + i] - W(pow(2, stage), i) * a[begin + i + pow(2, stage - 1)];
	}
}

void halfUp(vector<cd>& a, vector<cd>& b, bool firsthalf) {
	int stage{ int(log2(a.size())) };
	if (firsthalf) {
		for (int i = 0;i < pow(2, stage - 1)/2;i++) {
			b[i] = a[i] + W(pow(2, stage), i) * a[i + pow(2, stage - 1)];
		}
	}
	else {
		for (int i = pow(2, stage - 1) / 2;i< pow(2, stage - 1) ;i++) {
			b[i] = a[i] + W(pow(2, stage), i) * a[i + pow(2, stage - 1)];
		}
	}
}

void halfDown(vector<cd>& a, vector<cd>& b,bool firsthalf) {
	int stage{ int(log2(a.size())) };
	if (firsthalf) {
		for (int i = 0;i < pow(2, stage - 1)/2;i++) {
			b[i + pow(2, stage - 1)] = a[i] - W(pow(2, stage), i) * a[i + pow(2, stage - 1)];
		}
	}
	else {
		for (int i = pow(2, stage - 1) / 2;i < pow(2, stage - 1);i++) {
			b[i + pow(2, stage - 1)] = a[i] - W(pow(2, stage), i) * a[i + pow(2, stage - 1)];
		}
	}
}



int main() {
	ifstream In("input1024.txt");
	if (!In) {
		cout << "Fail to open file" << endl;
		return 0;
	}
	vector<cd> a;
	double d1;
	while (In >> d1) {
		a.emplace_back(cd(d1));
	}
	In.close();
	
	//sort vector a into bit reversed order
	vector<cd> a_temp;
	for (int i = 0;i < a.size();i++) {
		a_temp.emplace_back(a[bitReverse(i, a.size())]);
	}
	a = a_temp;

	vector<cd> b(a.size());
	
	//original iterative FFT
	{	
		Timer TIterSingle;
		iter_fft(a, b, a.size());
	}
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	
	//output0
	ofstream Out0("output_iter_single.txt");
	if (!Out0) {
		cout << "Fail to access file" << endl;
		return 0;
	}

	for (int i = 0;i < a.size();++i) {
		Out0 << b[i] << "\n";
	}
	Out0.close();
	//reset a b
	a = a_temp;
	for (auto& i : b) i = 0;

	//multi thread iterative FFT
	{
		Timer TIterMulti;
		for (int stage = 1;stage <= log2(a.size())-2 ;stage++) {
			int parts{ int(a.size() / pow(2, stage)) };
			thread t1(iter_fft_thread, ref(a), ref(b), a.size(), stage, 1, parts / 4);
			thread t2(iter_fft_thread, ref(a), ref(b), a.size(), stage, parts / 4 + 1, parts / 2);
			thread t3(iter_fft_thread, ref(a), ref(b), a.size(), stage, parts / 2 + 1, parts * 3 / 4);
			iter_fft_thread(a, b, a.size(), stage, parts * 3 / 4 + 1, parts);
			t1.join();
			t2.join();
			t3.join();
			
			for (int i = 0;i < a.size();i++) {
				a[i] = b[i];
			}

		}
		//second last stage
		thread t11(upLines, ref(a), ref(b), 0);
		thread t22(upLines, ref(a), ref(b), a.size()/2);
		thread t33(downLines, ref(a), ref(b), 0);
		downLines(a, b, a.size() / 2);
		t11.join();
		t22.join();
		t33.join();

		for (int i = 0;i < a.size();i++) {
			a[i] = b[i];
		}

		//last stage
		thread t111(halfUp, ref(a), ref(b), 1);
		thread t222(halfUp, ref(a), ref(b), 0);
		thread t333(halfDown, ref(a), ref(b), 1);
		halfDown(a, b, 0);
		t111.join();
		t222.join();
		t333.join();
	}
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	
	//output1
	ofstream Out1("output_iter_multi.txt");
	if (!Out1) {
		cout << "Fail to access file" << endl;
		return 0;
	}

	for (int i = 0;i < a.size();++i) {
		Out1 << b[i] << "\n";
	}
	Out1.close();
	//reset a b
	a = a_temp;
	for (auto& i : b) i = 0;

	//original recursive FFT
	{
		Timer TRecurSingle;
		recur_fft(a, b, 0, a.size()-1, a.size());
	}
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	
	//output2
	ofstream Out2("output_recur_single.txt");
	if (!Out2) {
		cout << "Fail to access file" << endl;
		return 0;
	}

	for (int i = 0;i < a.size();++i) {
		Out2 << b[i] << "\n";
	}
	Out2.close();
	//reset a b
	a = a_temp;
	for (auto& i : b) i = 0;

	//multi thread recursive FFT
	{
		Timer TRecurMulti;
		thread t1(recur_fft, ref(a), ref(b), 0, a.size() / 4 - 1, a.size());
		thread t2(recur_fft, ref(a), ref(b), a.size() / 4, a.size() / 2 - 1, a.size());
		thread t3(recur_fft, ref(a), ref(b), a.size() / 2, a.size() * 3 / 4 - 1, a.size());
		recur_fft(a, b, a.size() * 3 / 4, a.size() - 1, a.size());
		t1.join();
		t2.join();
		t3.join();
		
		for (int i = 0;i < a.size();i++) {
			a[i] = b[i];
		}

		//second last stage
		thread t11(upLines, ref(a), ref(b), 0);
		thread t22(upLines, ref(a), ref(b), a.size() / 2);
		thread t33(downLines, ref(a), ref(b), 0);
		downLines(a, b, a.size() / 2);
		t11.join();
		t22.join();
		t33.join();
		
		for (int i = 0;i < a.size();i++) {
			a[i] = b[i];
		}

		//last stage
		thread t111(halfUp, ref(a), ref(b), 1);
		thread t222(halfUp, ref(a), ref(b), 0);
		thread t333(halfDown, ref(a), ref(b), 1);
		halfDown(a, b, 0);
		t111.join();
		t222.join();
		t333.join();

	}
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;


	//output3
	ofstream Out3("output_recur_multi.txt");
	if (!Out3) {
		cout << "Fail to access file" << endl;
		return 0;
	}

	for (int i = 0;i < a.size();++i) {
		Out3 << b[i] << "\n";
	}
	Out3.close();
	//reset a b
	a = a_temp;
	for (auto& i : b) i = 0;


	return 0;

}