#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "get_time.h"
#include "sequence.h"
#include "parallel.h"
#include "utils.h"
#include "math.h"
#include <utility>
//using namespace std;
int threshold=256;

void upsweep (int*A,int s,int t){
    if (s==t){
        return;
    }
    if (t-s<=threshold){
        for (int i=s;i<t;i++)
            A[t]+=A[i];
        return;
    }
    int mid=(s+t)/2;
    cilk_spawn upsweep(A,s,mid);
    upsweep(A,mid+1,t);
    cilk_sync;
    A[t]+=A[mid];
}

void downsweep(int *A,int s,int t,int p){
    if (s==t){
        A[s]=p;
        return;
    }
    if (t-s<=threshold){
        int temp=A[s],newtemp;

        A[s]=p;

        for(int i=s+1;i<=t;i++){
            newtemp=temp;
            temp=A[i];
            A[i]=A[i-1]+newtemp;
        }
        return;
    }
    int mid=(s+t)/2;
    cilk_spawn downsweep(A,s,mid,p);
    downsweep(A,mid+1,t,p+A[mid]);
    cilk_sync;
}

int inplace_scan(int *A,int n){
    if (n<=threshold){
        int temp=A[0],newtemp;

        A[0]=0;

        for(int i=1;i<n;i++){
            newtemp=temp;
            temp=A[i];
            A[i]=A[i-1]+newtemp;
        }
        return A[n-1]+temp;
    }
    upsweep(A,0,n-1);
    int sigma=A[n-1];
    downsweep(A,0,n-1,0);
    return sigma;
}




int main(int argc,char ** argv){
    if(argc==1 or argc>4){
        std::cout<<"Command error"<<endl;
        exit(1);
    }
    int n=atoi(argv[1]);
    int total_times=1;
    if (argc>=3)
        total_times=atoi(argv[2]);
    if (argc==4)
        threshold=atoi(argv[3]);
    int * A=new int[n];
    int* B=new int[n];
    int *mf=new int[n];
    cilk_for(int i=0;i<n;i++) {
        A[i]=i;
        B[i]=0;
    }
    cilk_for(int i=0;i<n;i++) mf[i]=0;

    sequence::scan(A,B,n,plus<int>(),0);
    //for (int i=0;i<n;i++)
        //std::cout<<B[i]<<endl;
    double time=0,s=0;
    for(int i=0;i<total_times;i++){
        cilk_for(int j=0;j<n;j++) 
            A[j]=j;
       
        cilk_for(int j=0;j<n;j++) mf[j]=0;
        timer t; t.start();
        s=inplace_scan(A,n);
        t.stop();
        time+=t.get_total()/double(total_times);
    }
    std::cout<<time<<endl;

    cilk_for(int i=0;i<n;i++){
        if(B[i]!=A[i]){
            std::cout<<i<<" wrong"<<endl;
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