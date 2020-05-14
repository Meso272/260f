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
#define max(a,b) ((a) > (b) ? (a) : (b))
/*
void PKS(long long * A, unordered_map<long long, long long>H){
    unordered_map<long long, long long>R;
    while (!H.empty()){
        cilk_for(auto &p:H){
            long long i=iter->first;
            long long hi=iter->second;
            unordered_map<long long, long long>::fiter=R.find(i);
            if (fiter!=R.end()){
                long long ri=filter->second;
                if (ri<i)
                    R[i]=i;
            }
            else{
                R.insert(make_pair(i,i));
            }
            fiter=R.find(hi);
            if (fiter!=R.end()){
                long long ri=filter->second;
                if (ri<i)
                    R[hi]=i;
            }
            else{
                R.insert(make_pair(hi,i));
            }



        }

    }
}
*/
int main(){
    phmap::parallel_flat_hash_map<int,int> pfhm;
    for(int i=0;i<=1000000;i++){
        pfhm.insert(make_pair(i,i));
    }
    int sum=0;
    cilk_for(auto &p:pfhm){
        sum=sum+p.second;
    }
    std::cout<<sum<<std::endl;
}