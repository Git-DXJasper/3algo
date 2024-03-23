//HW7 by Dongze Xie
//SU Net ID: dxie03    SUID: 375402964

#include<iostream>
#include<fstream>
#include<unordered_map>
#include<map>
#include<vector>
#include<string>
#include<math.h>

using namespace std;

class Circuit {
public:
	string* cname;
	int ip, op;
	vector< pair<vector<int>, vector<int>> > tt; //truth table
	vector<vector<int>> uf; //unique form of output
	Circuit() {}//default constructor
	Circuit(string str) { cname = new string{ str }; }
};

class myFunctor {
public:
	bool operator()(const pair<int, int>& P1, const pair<int, int>& P2)const {
		return (P1.first == P2.first) ? (P1.second < P2.second) : (P1.first < P2.first);
	}
};

ostream& operator<<(ostream& str, const vector<pair<vector<int>, vector<int>>>& V) { //for cout tt
	for (auto& i : V) {
		for (auto& j : i.first) {
			str << j;
		}
		str << " ";
		for (auto& k : i.second) {
			str << k;
		}
		str << endl;
	}
	str << endl;
	return str;
}

template<>
class hash<Circuit> {//hash the sum of output part
public:
	size_t operator()(const Circuit& C)const {
		int sum{ 0 };
		for (auto& i : C.tt) {
			for (auto& j : i.second) {
				sum += j;
			}
		}
		return hash<int>{}(sum);
	}
};

template<>
class equal_to<Circuit> {//check if uf is exactly identical
public:
	bool operator()(const Circuit& C1, const Circuit& C2)const {
		vector<int> V1, V2;
		for (auto& i : C1.uf) {
			for (auto& j : i) {
				V1.push_back(j);
			}
		}
		for (auto& i : C2.uf) {
			for (auto& j : i) {
				V2.push_back(j);
			}
		}
		for (int i = 0;i < V1.size();i++) {
			if (V1[i] != V2[i]) return false;
		}
		return true;
	}
};

void Standardize(Circuit& C) {//generate uf and store in C.uf
	map<pair<int, int>, int, myFunctor> M_eigen;
	//(sum of entire row, take input as binary and convert into decimal)
	//,the row index of original tt 
	int index{ 0 };
	for (auto& row : C.tt) {
		int sum{ 0 };
		for (auto& i : row.first) {
			sum += i;
		}
		for (auto& j : row.second) {
			sum += j;
		}
		int digit{ C.ip - 1 };
		int dec{ 0 };
		for (auto& i : row.first) {
			dec += i * pow(2, digit);
			digit--;
		}
		M_eigen.insert({ make_pair(sum,dec),index });
		index++;
	}

	vector<vector<int>> temp_uf;
	for (auto& m : M_eigen) {
		temp_uf.push_back((C.tt[m.second]).second);
	}

	//sort by col sum of temp_uf
	vector<pair<int, int>> V_eigen;//(col, colsum) of temp_uf

	for (int col = 0; col < C.op;col++) {
		int colsum{ 0 };
		for (auto& i : temp_uf) {
			colsum += i[col];
		}
		V_eigen.push_back(make_pair(col, colsum));
	}

	for (int i = 0;i < V_eigen.size();i++) { //sort V_eigen
		for (int j = 0;j < V_eigen.size() - 1;j++) {
			if (V_eigen[j].second > V_eigen[j + 1].second) swap(V_eigen[j], V_eigen[j + 1]);
			j++;
		}
	}


	for (int rownum = 0;rownum < pow(2, C.ip);rownum++) { //rearrange by switch col, to finalize the real C.uf
		vector<int> row;
		for (int colnum = 0;colnum < C.op;colnum++) {
			row.push_back(temp_uf[rownum][(V_eigen[colnum].first)]);
		}
		C.uf.push_back(row);
	}
}

void Add(Circuit& C, unordered_map<Circuit, string*>& DB) {

	pair<unordered_map<Circuit, string*>::iterator, bool> ret;
	ret = DB.insert({ C,C.cname });
	if (ret.second) {//if insert succeeds
		cout << *C.cname << " is added to the database." << endl;
		cout << endl;
	}
	else {
		cout << *C.cname << " is not added." << endl;
		auto it = DB.find(C);
		cout << "It has a re-useable circuit, " << *((*it).first.cname) << " in the databse," << endl;
		cout << "whose truth table is shown below." << endl;
		cout << (*it).first.tt;
	}
}

void Find(Circuit& C, unordered_map<Circuit, string*>& DB) {

	auto it = DB.find(C);
	if (it != DB.end()) {//find succeeds
		cout << *C.cname << " has a re-useable circuit, " << *((*it).first.cname) << " , in the database," << endl;
		cout << "whose truth table is shown below." << endl;
		cout << (*it).first.tt;
	}
	else {
		cout << *C.cname << " does not have any re-useable circuit in the database." << endl;
		cout << endl;
	}
}

void Delete(Circuit& C, unordered_map<Circuit, string*>& DB) {

	auto it = DB.find(C);
	if (it != DB.end()) {//find succeeds
		cout << *C.cname << " has a re-useable circuit, " << *((*it).first.cname) << " , in the database," << endl;
		cout << "whose truth table is deleted from the database." << endl;
		cout << (*it).first.tt;
		DB.erase(it);

	}
	else {
		cout << *C.cname << " does not have any re-useable circuit in the database." << endl;
		cout << endl;
	}
}



int main() {
	unordered_map<Circuit, string*> DB;

	ifstream In("input.txt");
	vector<string> V_in; //vector of each line of the input file
	string str;
	while (In >> str) { V_in.push_back(str); }
	In.close();
	//Read in from input.txt
	vector<Circuit> V_cir; //vector of each circuit under operation
	for (auto it = V_in.begin(); it != V_in.end();it++) {
		if (*it == "Add") {
			//create a new Circuit object 
			it++;
			V_cir.push_back(*it);
			Circuit& cur = V_cir.back();
			//initialize the current Circuit object
			it++; cur.ip = stoi(*it);
			it++; cur.op = stoi(*it);

			for (int i = 0; i < pow(2, cur.ip);i++) {
				it++;
				vector<int> v1, v2;
				for (int j = 0; j < cur.ip;j++) {
					v1.push_back((int)((*it)[j]) - 48);
				}
				for (int k = 0; k < cur.op;k++) {
					v2.push_back((int)((*it)[(cur.ip) + k]) - 48);
				}
				cur.tt.push_back(make_pair(v1, v2));
			}
			Standardize(cur);

			//do Add operation on the current Circuit object
			Add(cur, DB);
		}

		if (*it == "Find") {
			//create a new Circuit object 
			it++;
			V_cir.push_back(*it);
			Circuit& cur = V_cir.back();
			//initialize the current Circuit object
			it++; cur.ip = stoi(*it);
			it++; cur.op = stoi(*it);

			for (int i = 0; i < pow(2, cur.ip);i++) {
				it++;
				vector<int> v1, v2;
				for (int j = 0; j < cur.ip;j++) {
					v1.push_back((int)((*it)[j]) - 48);
				}
				for (int k = 0; k < cur.op;k++) {
					v2.push_back((int)((*it)[(cur.ip) + k]) - 48);
				}
				cur.tt.push_back(make_pair(v1, v2));
			}
			Standardize(cur);

			//do Find operation on the current Circuit object
			Find(cur, DB);
		}

		if (*it == "Delete") {
			//create a new Circuit object 
			it++;
			V_cir.push_back(*it);
			Circuit& cur = V_cir.back();
			//initialize the current Circuit object
			it++; cur.ip = stoi(*it);
			it++; cur.op = stoi(*it);

			for (int i = 0; i < pow(2, cur.ip);i++) {
				it++;
				vector<int> v1, v2;
				for (int j = 0; j < cur.ip;j++) {
					v1.push_back((int)((*it)[j]) - 48);
				}
				for (int k = 0; k < cur.op;k++) {
					v2.push_back((int)((*it)[(cur.ip) + k]) - 48);
				}
				cur.tt.push_back(make_pair(v1, v2));
			}
			Standardize(cur);
			//do Delete operation on the current Circuit object
			Delete(cur, DB);
		}
	}

	return 0;
}