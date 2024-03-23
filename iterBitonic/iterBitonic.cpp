#include<iostream>
#include<vector>
#include <thread>

using namespace std;

void FrontUp(vector<int>& Data, int n1, int n2);
void FrontDown(vector<int>& Data, int n1, int n2);
void Sort(vector<int>& Data, int step, int partBegin, int partEnd);
void HalfUp(vector<int>& Data, int n1, int n2, int n);
void HalfDown(vector<int>& Data, int n1, int n2, int n);
void QuarterUp(vector<int>& Data, int n1, int n2, int n);




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

int main() {
    int n{ 33554432 }, m{ 10000 };
    vector<int> Data(n);
    for (auto& i : Data) i = rand() % m;
 

    {
        Timer TT;
        for (int step = 2;step <= n;step *= 2) {
            Sort(Data, step, 1, n / step);
        }
    }
    /*for (int i = 0;i < n;i++) {
        if (i % 10 == 0) cout << endl;
        cout << Data[i] << " ";
    }*/

    cout << endl;
    cout << "###################" << endl;
    cout << endl;

    for (auto& i : Data) i = rand() % m;
    
    {
        Timer T;
        for (int step = 2;step <= n / 4;step *= 2) {
            Sort(Data, step, 1, (n / step / 4));
            thread t1(Sort, ref(Data), step, (n / step / 4) + 1, (n / step / 4) * 2);
            thread t2(Sort, ref(Data), step, (n / step / 4) * 2 + 1, (n / step / 4) * 3);
            thread t3(Sort, ref(Data), step, (n / step / 4) * 3 + 1, (n / step));
            t1.join();
            t2.join();
            t3.join();
        }
        /*for (int step = n / 2;step <= n;step *= 2) { //handle the rest 2 steps(not parallel)
            Sort(Data, step, 1, n / step);
        }*/
        
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
            FrontUp(Data, 0, n / 4 - 1);
            thread t1(FrontUp, ref(Data), n / 4, n / 2 - 1);
            thread t2(FrontDown, ref(Data), n / 2, n / 4 * 3 - 1);
            thread t3(FrontDown, ref(Data), n / 4 * 3, n - 1);
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
            FrontUp(Data, 0, n / 4 - 1);
            thread t1(FrontUp, ref(Data), n / 4, n / 2 - 1);
            thread t2(FrontUp, ref(Data), n / 2, n / 4 * 3 - 1);
            thread t3(FrontUp, ref(Data), n / 4 * 3, n - 1);
            t1.join();
            t2.join();
            t3.join();
        }
    }

    /*for (int i = 0;i < n;i++) {
        if (i % 10 == 0) cout << endl;
        cout << Data[i] << " ";
    }*/


    return 0;
}

void FrontUp(vector<int>& Data, int n1, int n2) {
    for (int step0 = (n2 + 1 - n1) / 2;step0 >= 1;step0 /= 2) {//each iteration is a stage
        for (int part0 = 1;(part0 * step0 * 2) <= n2 - n1 + 1;++part0) {
            for (int i = 0;i < step0;i++) {
                if (Data[n1+(part0 - 1) * step0 * 2+i] > Data[n1+(part0 - 1) * step0 * 2 + step0+i]){
                    swap(Data[n1+(part0 - 1) * step0 * 2+i],Data[n1+(part0 - 1) * step0 * 2 + step0+i]);
                }
            }
        }
    }
}

void FrontDown(vector<int>& Data, int n1, int n2) {
    for (int step0 = (n2 + 1 - n1) / 2;step0 >= 1;step0 /= 2) {//each iteration is a stage
        for (int part0 = 1;(part0 * step0 * 2) <= n2 - n1 + 1;++part0) {
            for (int i = 0;i < step0;i++) {
                if (Data[n1+(part0 - 1) * step0 * 2+i] < Data[n1+(part0 - 1) * step0 * 2 + step0+i]) {
                    swap(Data[n1+(part0 - 1) * step0 * 2+i], Data[n1+(part0 - 1) * step0 * 2 + step0+i]);
                }
            }
        }
    }
}

void Sort(vector<int>& Data, int step, int partBegin, int partEnd) {
    for (int part = partBegin;part <= partEnd;part++) {
        if (part % 2 == 1) FrontUp(Data, step * (part - 1), step * (part - 1) + step - 1);
        else FrontDown(Data, step * (part - 1), step * (part - 1) + step - 1);
    }
}

void HalfUp(vector<int>& Data, int n1, int n2,int n) {
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

