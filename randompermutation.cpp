#include<unordered_map>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "get_time.h"
#include "sequence.h"
#include<utility>
#include<mutex>
//#include "parallel.h"
//#include "utils.h"
#include "math.h"
#include "parallel_hashmap/phmap.h"
#define threshold 4096
#define max(a,b) ((a) > (b) ? (a) : (b))
#define NMSP phmap
#define MTX std::mutex
#define prefix_rate 0.02
#define EXTRAARGS , NMSP::container_internal::hash_default_hash<K>, \
                            NMSP::container_internal::hash_default_eq<K>, \
                            std::allocator<std::pair<const K, V>>, 4, MTX

#define MAPNAME phmap::parallel_flat_hash_map
template <class K, class V>
using HashT      = MAPNAME<K, V EXTRAARGS>;

using hash_t     = HashT<int64_t, int64_t>;
using str_hash_t = HashT<const char *, int64_t>;


void PKS(long long * A, pair<long long,long long> * H, long long n){
    phmap::parallel_flat_hash_map<long long ,long > R;
    long long size=long long (double(n)*prefix_rate);
    long long rest_swaps=n;
    pair<long long,long long> *filter_res=new long long [size];
    if (size<threshold)
        size=threshold;
    while (rest_swaps>0){
        if(rest_swaps<=threshold){
                for(long long  i=rest_swaps;i>=0;i--){
                    pair<long long ,long long > swp=H[i];
                    swap(A[swp.first],A[swp.second])
                }
                break;
        }
        if(size>rest_swaps){
            size=rest_swaps;
        }
        long long start=rest_swaps-size;
        long long *sH=H+start;

        cilk_for(long long j=0;j<rest_swaps;j++){
            

            pair<long long ,long long > swp=sH[j];
            long long i=swp.first;
            long long hi=swp.second;
            parallel_flat_hash_map<long long ,long long>::iterator fiter=R.find(i);
            if (fiter!=R.end()){
                long long ri=fiter->second;
                if (ri<i)
                    R[i]=i;
            }
            else{
                R.insert(make_pair(i,i));
            }
            fiter=R.find(hi);
            if (fiter!=R.end()){
                long long ri=fiter->second;
                if (ri<i)
                    R[hi]=i;
            }
            else{
                R.insert(make_pair(hi,i));
            }



        }
        cilk_for(long long j=0;j<rest_swaps;j++){
            pair<long long ,long long > swp=sH[j];
            long long i=swp.first;
            long long hi=swp.second;
            parallel_flat_hash_map<long long ,long long>::iterator fiter=R.find(i);
            parallel_flat_hash_map<long long ,long long>::iterator fhiter=R.find(hi);
            if(fiter!=R.end() and fiter->second==i and fhiter!=R.end() and fhiter->second==i){
                swap(A[hi],A[i]);
                sH[j].first=-1;
            }


        }
        auto pred=[&](pair<long long ,long long > x)->bool{return x.first==-1;};
        long long failednum=sequence::filterf(sH,filter_res,size,pred);
        rest_swaps=rest_swaps-size+failednum;
        cilk_for(long long j=0;j<failednum;j++){
            sH[j]=filter_res[j];
        }

    }   


    

}

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
    pair<long long ,long long > H=new long long[n];
    int *mf=new int[n];
    
    //cilk_for(long long i=0;i<n;i++) mf[i]=0;
    //timer t0; t0.start();
    //sequence::scan(A,B,n,plus<int>(),0);
    //t0.stop();
    //std::cout<<t0.get_total()<<std::endl;

    //for (int i=0;i<n;i++)
        //std::cout<<B[i]<<endl;
    double time=0;
    long long s=0;
    for(int i=0;i<total_times;i++){
        cilk_for(long long j=0;j<n;i++) {
            A[j]=j;
            H[j]=(j,(j+55)%n);
        }
       
        cilk_for(long long j=0;j<n;j++) mf[j]=0;
        timer t; t.start();
        s=PKS(A,H,n);
        t.stop();
        time+=t.get_total()/double(total_times);
    }
    std::cout<<time<<std::endl;

    //parallel_for(int i=0;i<n;i++){
      //  if(B[i]!=A[i]){
        //    std::cout<<i<<" wrong"<<std::endl;
            //break;
        //}
    //}
    //if (B[n-1]!=s){
    //    std::cout<<"last wrong"<<endl;
    //}
    delete []A;
    delete []H;
    delete []mf;
    return 0;

}