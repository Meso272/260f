// This code is part of the Problem Based Benchmark Suite (PBBS)
// Copyright (c) 2010 Guy Blelloch and the PBBS team
//
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

#include <iostream>
#include <algorithm>
#include "gettime.h"
#include "parallel.h"
#include "IO.h"
#include "parseCommandLine.h"
#include "randPerm.h"

using namespace std;
using namespace benchIO;

void timeRandPerm(intT n, int rounds, intT ratio) {
  intT* A = newA(intT, n);
  parallel_for(intT i=0;i<n;i++) A[i] = i;
  randPerm(A, n, ratio);
  for (int i=0; i < rounds; i++) {
    parallel_for(intT i=0;i<n;i++) A[i] = i;
    startTime();
    randPerm(A, n, ratio);
    nextTimeN();
  }
  free(A);
  cout << endl;
}

int parallel_main(int argc, char* argv[]) {
  commandLine P(argc,argv,"[-p] [-o <outFile>] [-r <rounds>] <inFile>");
  int rounds = P.getOptionIntValue("-r",1);
  intT ratio = P.getOptionIntValue("-ratio",-1);
  intT n = atoi(P.getArgument(0));
  timeRandPerm(n, rounds, ratio);
}
