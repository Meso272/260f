#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "get_time.h"
#include "sequence.h"
//#include "myparallel.h"
#include "utils.h"
#include "math.h"
#include <utility>
//using namespace std;

 int threshold=256;
/*
void myupsweep (long long *A,long long  s,long long t){
    if (s==t){
        return;
    }
    if (t-s+1<=threshold){
        for (long long i=s;i<t;i++)
            A[t]+=A[i];
        return;
    }
    long long mid=(s+t)/2;
    cilk_spawn myupsweep(A,s,mid);
    myupsweep(A,mid+1,t);
    cilk_sync;
    A[t]+=A[mid];
}

void mydownsweep(long long *A,long long s,long long t,long long p){
    if (s==t){
        A[s]=p;
        return;
    }
    if (t-s<=threshold){
        long long temp=A[s],newtemp;

        A[s]=p;

        for(long long i=s+1;i<=t;i++){
            newtemp=temp;
            temp=A[i];
            A[i]=A[i-1]+newtemp;
        }
        return;
    }
    long long mid=(s+t)/2,temp=A[mid];
    cilk_spawn mydownsweep(A,s,mid,p);
    mydownsweep(A,mid+1,t,p+temp);
    cilk_sync;
}

long long my_inplace_scan(long long *A,long long n){
    if (n<=threshold){
        long long temp=A[0],newtemp;

        A[0]=0;

        for(long long i=1;i<n;i++){
            newtemp=temp;
            temp=A[i];
            A[i]=A[i-1]+newtemp;
        }
        return A[n-1]+temp;
    }
    myupsweep(A,0,n-1);
    long long sigma=A[n-1];
    mydownsweep(A,0,n-1,0);
    return sigma;
}

*/


int main(int argc,char ** argv){
    if(argc==1 or argc>4){
        std::cout<<"Command error"<<std::endl;
        exit(1);
    }
    long long n=atoi(argv[1]);
    int total_times=1;
    if (argc>=3)
        total_times=atoi(argv[2]);
    if (argc==4)
        threshold=atoi(argv[3]);
    long long * A=new long long[n];
    long long* B=new long long[n];
    int *mf=new int[n];
    cilk_for(long long i=0;i<n;i++) {
        A[i]=i;
        B[i]=0;
    }
    cilk_for(long long i=0;i<n;i++) mf[i]=0;
    timer t0; t0.start();
    sequence::scan(A,B,n,plus<int>(),0);
    t0.stop();
    std::cout<<t0.get_total()<<std::endl;

    //for (int i=0;i<n;i++)
        //std::cout<<B[i]<<endl;
    double time=0;
    long long s=0;
    for(int i=0;i<total_times;i++){
        cilk_for(long long j=0;j<n;j++) 
            A[j]=j;
       
        cilk_for(long long j=0;j<n;j++) mf[j]=0;
        timer t; t.start();
        s=sequence::my_inplace_scan(A,n);
        t.stop();
        time+=t.get_total()/double(total_times);
    }
    std::cout<<time<<std::endl;

    parallel_for(int i=0;i<n;i++){
        if(B[i]!=A[i]){
            std::cout<<i<<" wrong"<<std::endl;
            break;
        }
    }
    //if (B[n-1]!=s){
    //    std::cout<<"last wrong"<<endl;
    //}
    delete []A;
    delete []B;
    delete []mf;
    return 0;

}