#include <iostream>
#include <vector>
#include <cstdlib>
#include <thread>
#include <algorithm>

#include <math.h>

using namespace std;

//Shared memory between threads
vector<int> data;
vector<int> data2;
vector<int> sum;

//Function to create random int data
vector<int> create_data(int n){
    vector<int> v(n);
    for(vector<int>::iterator it = v.begin() ; it!= v.end(); ++it){
        *it = rand()%1000000;
    }
    return v;
}
//Funtion to print the data on console
void show(vector<int> &v){
    //int n = v.size();
    for(vector<int>::iterator it = v.begin() ; it!= v.end(); ++it){
        cout << *it << " ";
    }
    cout << endl;
}

void thread_sum(int t_id,int p){
    //Thread will figure it out on which part it have to operate
    int n = data.size();
    int start = t_id*(n/p);
    int end = (n>(t_id+1)*(n/p))?(t_id+1)*(n/p):n;
    for(int i=start+1;i<end;i++){
        data[i] = data[i-1]+data[i];
    }
    sum[t_id] = data[end-1];
}

void thread_sum_final(int t_id,int p){
    //Thread will figure it out on which part it have to operate
    int n = data.size();
    int start = t_id*(n/p);
    int end = (n>(t_id+1)*(n/p))?(t_id+1)*(n/p):n;
    for(int i=start;i<end;i++){
        data[i] = sum[t_id-1]+data[i];
    }
}

//Function for doing prefix sum
void prefix_sum(){
    int n = (int)data2.size();
    for(int  i=1;i<n;i++){
        data2[i] = data2[i] + data2[i-1];
    }
}

int main(){
	int element_size = 50000000;
    data = create_data(element_size);
    data2 = data;
    clock_t startTime;
    
    
    
    /************************Sequential Implementation Start***************************/
    startTime = clock();
    prefix_sum();
    cout << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;
    /***********************Sequential Implementation End******************************/
    
    /***********************Parallel Impementation Start*****************************/
    //Number of threads to create
    startTime = clock();
    int p = 4;
    sum = vector<int>(p);
    //Creation and distribution of data to threads
    vector<thread> threads;
    for(int i=1;i<p;i++){
        //Pushing thread while creating it
        threads.push_back(thread(thread_sum,i,p));
    }
    //Own work
    int t_id=0;
    int n = data.size();
    int start = t_id*(n/p);
    int end = (n>(t_id+1)*(n/p))?(t_id+1)*(n/p):n;
    for(int i=start+1;i<end;i++){
        data[i] = data[i-1]+data[i];
    }
    sum[t_id] = data[end-1];
    for_each(threads.begin(),threads.end(),[](thread &t){t.join();});
    //Serial work
    for(int i=1;i<p;i++){
        sum[i] = sum[i]+sum[i-1];
    }
    threads.clear();
    for(int i=1;i<p;i++){
        //Pushing thread while creating it
        threads.push_back(thread(thread_sum_final,i,p));
    }
    for_each(threads.begin(),threads.end(),[](thread &t){t.join();});
    cout << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;
    /************************Parallel Implementation End*******************************/
    
	for(int i=0;i<(int)data.size();i++){
        if(data[i]!=data2[i]){
            cout << "Result dont match" << endl;
            break;
        }
    }
    cout << "Results are perfectly fine" << endl;
    
    return 0;
}
