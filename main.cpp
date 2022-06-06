/*
"De ce merg aib-urile?"

Arborii indexati binar sunt o structura de date arborescenta care permit rezolvarea eficienta a urmatoarelor operatii:

1. update(pos, val) -> elementul de pe pozitia pos
2. query(pos) -> suma elementelor prefixului [1, pos]

Putem observa ca o ca un arbore de intervale poate rezolva aceste operatii, dar stiind ca trebuie sa aflam suma doar pentru prefixe il putem simplifica.

1 2 3 4 5 6 7 8
[-------------]
[-----] [-----]
[-] [-] [-] [-]
* * * * * * * * 

Astfel, ii vom "sterge" toti fii drepti.

1 2 3 4 5 6 7 8
[-------------] 1000
[-----]         x100
[-]             xx10
*   *   *   *   xxx1

Vom obtine mai multe intervale [st, dr] de lungime lsb(dr), care au capetele drepte distincte doua cate doua!

lsb(x) = cea mai mica putere a lui 2 care contribuie in reprezentarea lui x in baza 2

Folosindu-ne de aceasta observatie putem retine intr-un tablou suma elementelor din fiecare interval:
tree[dr] = sum(v[i], i = {dr - lsb(dr) + 1..dr})

Acum vom analiza calcularea eficienta lui lsb(x).
Putem sa parcurgem toti bitii lui x incepand cu cel mai putin semnificativ, cautand primul bit setat.
Este destul de ineficienta aceasta abordare complexitatea fiind O(log(x))
Ne putem folosi de operatiile pe biti pentru a avea o complexitate constanta.

x:      aaaaaaaa1000000
x-1:    aaaaaaaa0111111
~(x-1): bbbbbbbb1000000
b = !a

x:      aaaaaaaa1000000
~(x-1): bbbbbbbb1000000
----------------------- (&)
lsb(x): 000000001000000

lsb(x) = x & ~(x-1) -> poate da overflow

~(x-1) = -x => lsb(x) = x & -x

*/

#include <bits/stdc++.h>

using namespace std;

ifstream fin("elmaj.in");
ofstream fout("elmaj.out");

template <typename T>
struct Fenwick {
	int N;
	int LGN;
	vector<T> tree;

	Fenwick () {

	}

	Fenwick (int size) {
		N = size;
		LGN = __lg(N);
		tree = vector<T> (size);
	}

	Fenwick& operator = (const Fenwick &_) {
		N = _.N;
		LGN = _.LGN;
		tree = _.tree;
		return *this;
	}

	int lsb(int i) {
		return i & (-i);
	}

	void update(int position, int value) {
		for(int i = position; i <= N; i += lsb(i)) {
			tree[i] += value;
		}
	}

	int query(int position) {
		int ret = 0;
		for(int i = position; i > 0; i -= lsb(i)) {
			ret += tree[i];
		}
		return ret;
	}

	int query(int left, int right) {
		return query(right) - query(left - 1);
	}

	int search(int value) {
		int sum = 0, pos = 0, ret = 0;
		for(int i = LGN; i >= 0; i--) {
			if(pos + (1 << i) <= N) {
				if(sum + tree[pos + (1 << i)] <= value) {
					sum += tree[pos + (1 << i)];
					pos += (1 << i);

					ret = pos;
				}
			}
		}

		return ret;
	}
};

struct Query {
	int TYPE;
	int value;
};
vector<Query> Queries;

vector<int> values;
int normalize(int value) {
	return distance(values.begin(), lower_bound(values.begin(), values.end(), value)) + 1;
}

int N;
vector<int> freq;
Fenwick<int> DS;

const int error_message = -1;
const int INSERT = 1;
const int ERASE = 2;
const int QUERY = 3;

void Insert(int value) {
	N++;
	freq[value]++;
	DS.update(value, 1);
}

void Erase(int value) {
	assert(freq[value] > 0);
	N--;
	freq[value]--;
	DS.update(value, -1);
}

int MajorityQuery() {
	int value = DS.search(N / 2) + 1;

	// cout << (N + 1) / 2 << " " << value << " " << values[value - 1] << " " << DS.query(value) << '\n';

	if(value == 0) {
		return error_message;
	}

	if(freq[value] > N / 2) {
		return values[value - 1];
	}
	return error_message;
}

int main() {
	int Q;
	fin >> Q;
	Queries = vector<Query> (Q);

	for(auto &q: Queries) {
		int TYPE, value = 0;
		fin >> TYPE;

		assert(TYPE == INSERT || TYPE == ERASE || TYPE == QUERY);

		if(TYPE != QUERY) {
			fin >> value;
		}

		q = {TYPE, value};
	}

	for(const auto &q: Queries) {
		if(q.TYPE != QUERY) {
			values.push_back(q.value);
		}
	}
	sort(values.begin(), values.end());
	values.erase(unique(values.begin(), values.end()), values.end());
	for(auto &q: Queries) {
		if(q.TYPE != QUERY) {
			q.value = normalize(q.value);
		}
	}

	freq = vector<int> (values.size() + 1);
	DS = Fenwick<int> (values.size() + 1);

	for(const auto &q: Queries) {
		if(q.TYPE == INSERT) {
			Insert(q.value);
		} else if(q.TYPE == ERASE) {
			Erase(q.value);
		} else {
			fout << MajorityQuery() << '\n';
		}
	}
	return 0;
}
