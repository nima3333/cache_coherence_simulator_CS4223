//============================================================================
// Name        : multicore_coherency.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <bitset>
#include <array>
#include <list>
#include <unordered_set>

using namespace std;

enum instruction {Load=0, Store=1, Computation=2};
typedef pair<int, long> Operation;

typedef bitset<2> State;

struct cache_block {
	State state;
	int tag;
};

class Cache {
  public:
	Cache(int c_size, int asso, int b_size){
	    cache_size = c_size;
	    associativity = asso;
	    block_size = b_size;
		initialize_cache(cache_size, associativity, block_size);
		N = (int)ceil(log2(block_size/4));
		M = (int)ceil(cache_size/(block_size*associativity));
		printf("N = %d, M = %d", N, M);
	}

	int loadAddress(long address){
	    //Return nb of cycles to wait
	    //TODO: get tags
	    int tag = 0;
	    int index = 0;
	    //Check if exists in cache
	    if(cache_content[index].find(tag) != cache_content[index].end()){  //Present

	    }
	    else{ //Not present

	    }
        return 0;
	}
	int writeAddress(long address){
	    return 0;
	}

  private:
	vector<list<cache_block>> cache;
    vector<unordered_set<int>> cache_content;
	int cache_size;
	int associativity;
	int block_size;
	int N;
	int M;
	int initialize_cache(int cache_size, int associativity, int block_size){
		int nb_cache_blocs = cache_size / (block_size * associativity);
		for(int i=0; i<nb_cache_blocs; i++){
			cache.push_back(list<cache_block>());
            cache_content.push_back(unordered_set<int>());
		}
		cache_block a;
		a.state=1;
		a.tag=4;
		cache[1].push_back(a);
		return 0;
	}

};


class Core {
	public:
		Core(int core_id): core_number(core_id){
			//TODO: logic to handle failure
			fill_instruction_buffer();
			Cache cachee(1024, 2, 4);
		}

		int fill_instruction_buffer(){
			//Get the proper file name, let's assume we do not use C++20
				//string file_name = format("/file/{}_{}.data", program, core_number);    C++20 way
			string filename = string("../files/") + program + string("_") + to_string(core_number) + string(".data");

			//Loading the file into a vector
			ifstream myfile(filename);
			if (myfile.is_open())
			{
				cout << "Loading core" << endl;
				string line;
				while ( getline (myfile,line) )
				{
				  string line_1= line.substr(0, line.find(" "));
				  int operation = atoi(line_1.c_str());

				  string line_2= line.substr(line.find(" "), line.length());
				  long argument = stoul(line_2, nullptr, 16);

				  instruction_buffer.push(make_pair(operation, argument));
				}
				myfile.close();
				cout << "Loading achieved" << endl;
				return 1;
			}
			else{
				cout << "Unable to open file";
				return 0;
			}
		}

		int next_cycle(){
			return 0;
		}

	private:
		//Core attributes
		int core_number;  //To load the correct file
		string program{"bodytrack"};
		int store_counter{0}; //Store the number of stores
		int load_counter{0};
		int computing_counter{0};
		queue<Operation> instruction_buffer;
		//State


};


class Bus {

};

class BusMessage {

};


int main() {
	cout << "Hello World!!!" << endl; // prints !!!Hello World!!!
	Cache cachee(1024, 1, 16);

	return 0;

}
