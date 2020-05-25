// This code is part of the paper "A Simple and Practical Linear-Work
// Parallel Algorithm for Connectivity" in Proceedings of the ACM
// Symposium on Parallelism in Algorithms and Architectures (SPAA),
// 2014.  Copyright (c) 2014 Julian Shun, Laxman Dhulipala and Guy
// Blelloch
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights (to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "parallel.h"
#include "utils.h"
#include "randPerm.h"
#include <iostream>
#include "sequence.h"
//#include "speculative_for2.h"
#include "ndHash.h"
#include "speculative_for.h"
using namespace std;

inline unsigned int hashI(unsigned int a)
{
   a = (a*0x7ed55d16) + (a<<12);
   a = (a^0xc761c23c) ^ (a>>19);
   return a;
}

template <class E>
struct randPermStep {
  Table<hashSimplePair, intT> *dataCheck;
  intT n;
  intT* A;

  inline bool reserve(intT i) {
    dataCheck->insert(make_pair(i+hashI(i)%(n-i), i));
    return 1;
  }

  inline bool commit (intT i) {
    intT h = i+hashI(i)%(n-i);
//	if (i<2000000 && (dataCheck->find(h).second != i || (dataCheck->find(i).first != -1 && dataCheck->find(i).second != i)))
//		cout << i << ' ' << h << ' ' << dataCheck->find(i).second << ' ' << dataCheck->find(h).second << endl;
	//if (i % 100000 == 0) 
	//	cout << i << ' ' << h << ' ' << dataCheck->find(h).first << ' ' << dataCheck->find(h).second << ' ' << dataCheck->find(i).first << ' ' << dataCheck->find(i).second << endl;
	auto ret = dataCheck->find(h);
    if(ret.second == i) {
		auto check = dataCheck->find(i);
		if (check.first == INT_T_MAX || check.second == i) {
			swap(A[i], A[h]);
			return 1;
      }
    }
    return 0;
  }
};

template <class E>
void randPerm(E *A, intT n, intT r) {
  r = 50;
  randPermStep<E> rStep;//(H, &dataCheck);
  rStep.n = n;
  rStep.A = A;
  Table<hashSimplePair, intT> table(n / r, hashSimplePair()); 
  rStep.dataCheck = &table;
  speculative_for(rStep, 0, n, r, rStep.dataCheck, 0);
  
  rStep.dataCheck->del();
}

template void randPerm<intT>(intT*,intT,intT);
//template void randPerm<double>(double*,intT,intT);
