#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "get_time.h"
#include "sequence.h"
//#include "parallel.h"
//#include "utils.h"
#include "math.h"
#pragma pack(1)
template <class PRED>
long long inplace_filter(long long *A,long long n,PRED p){
    long long k=(long long)(sqrt(n));
    //std::cout<<k<<std::endl;
    long long packstart=0;
    long long *B=new long long[k];
    long long start=0;
    bool *fl=new bool[k];
    while(n-start>=k){
       
        long long m=sequence::filter(A+start,B,fl,k,p);
        for(long long j=0;j<m;j++){
            A[packstart+j]=B[j];
        }
        packstart+=m;
        start+=k;
    }
    //std::cout<<start<<std::endl;
    //std::cout<<packstart<<std::endl;
    if(start<n){
        long long length=n-start;
        //std::cout<<length<<std::endl;
        long long m=sequence::filter(A+start,B,fl,length,p);
         //std::cout<<m<<std::endl;
        for(long long j=0;j<m;j++){
            A[packstart+j]=B[j];
        }
        packstart+=m;
    }
    delete []B;
    delete []fl;
    return packstart;


}

int main(int argc,char ** argv){
    if(argc==1 or argc>3){
        std::cout<<"Command error"<<std::endl;
        exit(1);
    }
    long long n=atoi(argv[1]);
    int total_times=1;
    if (argc>=3)
        total_times=atoi(argv[2]);
    //if (argc==4)
      //  threshold=atoi(argv[3]);
    long long * A=new long long[n];
    long long* B=new long long[n];
    int *mf=new int[n];
    cilk_for(long long i=0;i<n;i++) {
        A[i]=i;
        B[i]=0;
    }
    cilk_for(long long i=0;i<n;i++) mf[i]=0;
    auto p=[&](int x){return x&1;};
    timer t0; t0.start();
    long long m0=sequence::filter(A,B,n,p);
    long long m1;
    t0.stop();
    std::cout<<t0.get_total()<<std::endl;

    //for (int i=0;i<n;i++)
        //std::cout<<B[i]<<endl;
    double time=0;
    for(int i=0;i<total_times;i++){
        cilk_for(long long j=0;j<n;j++) 
            A[j]=j;
       
        cilk_for(long long j=0;j<n;j++) mf[j]=0;
        timer t; t.start();
        m1=inplace_filter(A,n,p);
        t.stop();
        time+=t.get_total()/double(total_times);
    }
    std::cout<<time<<std::endl;
    if(m0!=m1){
        std::cout<<"wrong"<<std::endl;
    }
    else{
         cilk_for(long long i=0;i<m0;i++){
         if(B[i]!=A[i]){
            std::cout<<i<<" wrong"<<std::endl;
            //break;
            }
        }
  
    }
    
    delete []A;
    delete []B;
    delete []mf;
    return 0;

}