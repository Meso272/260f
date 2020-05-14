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
#define NMSP phmap
#define MTX std::mutex
#define EXTRAARGS , NMSP::container_internal::hash_default_hash<K>, \
                            NMSP::container_internal::hash_default_eq<K>, \
                            std::allocator<std::pair<const K, V>>, 4, MTX

#define MAPNAME phmap::parallel_flat_hash_map
template <class K, class V>
using HashT      = MAPNAME<K, V EXTRAARGS>;

using hash_t     = HashT<int64_t, int64_t>;
using str_hash_t = HashT<const char *, int64_t>;

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
    hash_t pfhm;
    cilk_for(int i=0;i<=1000000;i++){
        pfhm.insert(make_pair(i,i));
    }
    int sum=0;
    for(auto &p:pfhm){
        sum=sum+p.second;
    }
    std::cout<<sum<<std::endl;
}