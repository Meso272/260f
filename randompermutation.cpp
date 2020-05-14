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
void PKS(long long * A, long long * H, long long n){
    phmap::parallel_flat_hash_map<long long ,long > R;

    long long rest_swaps=n;
    while (rest_swaps>0){
        cilk_for(long long i=0;i<rest_swaps;i++){
            long long i=iter->first;
            long long hi=iter->second;
            parallel_flat_hash_map<long long ,long >::iterator filter=R.find(i);
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
        cilk_for(long long i=0;i<rest_swaps;i++){

        }


    }

}
*/
int main(){
    phmap::parallel_flat_hash_map<long long ,long long, boost::mutex> pfhm;
    cilk_for(int i=0;i<=1000000;i++){
        pfhm.insert(make_pair(i,i));
    }
    int sum=0;
    for(auto &p:pfhm){
        sum=sum+p.second;
    }
    std::cout<<sum<<std::endl;
}