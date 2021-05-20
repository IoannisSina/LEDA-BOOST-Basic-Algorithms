#include "LEDA/graph/graph.h"
#include "LEDA/graph/ugraph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/core/queue.h"
#include "LEDA/core/list.h"
#include "LEDA/core/array.h"
#include "LEDA/system/timer.h"


#include <iostream>
#include <string>

//my headers for graphs and functions
#include "graphs_functions.h"
#include "my_functions.h"

using namespace leda;


//functions for times 
void peirama_emfolevmena_tetragona();
void peirama_daktylios();
void peirama_4_epipeda();
void peirama_my_graph();
void times_function(ugraph& G);					//runs my_bipar_checker and Is_Bipartite for the graph G 1000 times and divides time by 1000



int main(){


	//uncomment one of the next calls to see results
	
	peirama_emfolevmena_tetragona();
	//peirama_daktylios();
	//peirama_4_epipeda();
	//peirama_my_graph();
	return 0;
}

void peirama_emfolevmena_tetragona(){
	
	//-----------------------Peiramatikh sigkrish gia grafhmata emfolevmenwn tetragwnwn----------------
	std::cout<<"--------------------------Emfolevmena tetragwna--------------------------\n\n\n";
	
	int peirama1[3] = {10000, 40000, 90000};		//number of nodes given
	
	for(int i=0; i < 3; i++){
		
		ugraph G;
		std::cout<<"Arithmos koryfwn: "<<peirama1[i]<<"\n\n";
		first_family(G , peirama1[i]);
		times_function(G);							//emfolevmena tetragwna
		
	}
								
	std::cout<<"-------------------------------------------------------------------------\n";
	//-------------------------------------------------------------------------------------------------
}

void peirama_daktylios(){
	
	//-----------------------Peiramatikh sigkrish gia grafhmata daktyliou----------------
	
	std::cout<<"--------------------------Daktylios--------------------------\n\n\n";
	
	
	int peirama2[3] = {10001, 40001, 90001};		//number of nodes given
	
	for(int i=0; i < 3; i++){
		
		ugraph G;
		std::cout<<"Arithmos koryfwn: "<<peirama2[i]<<"\n\n";
		second_family(G , peirama2[i]);
		times_function(G);				//emfolevmena tetragwna
		
	}
								
	std::cout<<"-------------------------------------------------------------\n";
	//-----------------------------------------------------------------------------------
}

void peirama_4_epipeda(){
	
	//-----------------------Peiramatikh sigkrish gia grafhmata 2k akmwn-----------------------
	
	std::cout<<"--------------------------2k akmes kai 4 epipeda--------------------------\n\n\n";
	
	int peirama3[3] = {500, 1000, 1500};		//number of nodes given
	
	for(int i=0; i < 3; i++){
		
		ugraph G;
		std::cout<<"k = "<<peirama3[i]<<" -> Arithmos koryfwn -> 4 * k = "<<peirama3[i]*4<<"\n\n";
		third_family(G , peirama3[i]);
		times_function(G);				//emfolevmena tetragwna
		
	}
								
	std::cout<<"--------------------------------------------------------------------------\n";
	//-----------------------------------------------------------------------------------------
}

void peirama_my_graph(){
	
	//----------------------------Peiramatikh sigkrish to diko mou grafhma----------------------------
	
	
	std::cout<<"--------------------------2k+4 koryfes 4k+4 akmes--------------------------\n\n\n";
	
	
	int peirama4[3] = {10000, 15000, 30000};		//number of nodes given
	
	for(int i=0; i < 3; i++){
		
		ugraph G;
		std::cout<<"k = "<<peirama4[i]<<" -> Arithmos koryfwn -> 2*k+4 = "<<(peirama4[i]*2)+4<<"\n\n";
		fourth_family(G , peirama4[i]);
		times_function(G);						//emfolevmena tetragwna
		
	}
								
	std::cout<<"---------------------------------------------------------------------------\n";
	//------------------------------------------------------------------------------------------------
}

void times_function(ugraph& G){
	
	
	timer my_time, leda_time;
	
	//--------------------------------------my_bipar_checker------------------------------
	
	list<node> my_V1;					
	list<node> my_V2;
	bool my_is_bipar;
	my_time.start();
	for(int i=0; i < 1000; i++){									//run algorithm 1000 times and divide by 1000 for better precsision
		my_is_bipar = my_bipar_checker(G, my_V1, my_V2);
		
		if(i != 999){												//dont clear lists at the last iteration so we can print the result 
			
			my_V1.clear();											//clear list to be emtpy for next iteration
			my_V2.clear();											//clear list to be emtpy for next iteration
		}
		
	}
	my_time.stop();
	std::cout<<"\tMy time: "<<my_time.elapsed_time()/1000<<"\n";		//divide by 1000
	//print_lists(G, my_V1, my_V2, my_is_bipar, "My");
	
	
	//------------------------------------------------------------------------------------
	std::cout<<"\n\n";
	
	//--------------------------------------LEDA's_bipar_checker--------------------------
	Make_Bidirected(G);						//make G bidirected (insert a reversal edge for each edge) so its equal to undirected G
	G.make_directed();						//make G directed for leda's function to run
	
	list<node> leda_V1;
	list<node> leda_V2;
	bool leda_is_bipar;
	leda_time.start();
	for(int i=0; i < 1000; i++){											//run algorithm 1000 times and divide by 1000 for better precsision
		leda_is_bipar = Is_Bipartite(G, leda_V1, leda_V2);	
		if(i != 999){														//dont clear lists at the last iteration so we can print the result 

			leda_V1.clear();												//clear list to be emtpy for next iteration
			leda_V2.clear();												//clear list to be emtpy for next iteration
		}
	}
	leda_time.stop();
	std::cout<<"\tLEDAS's time: "<<leda_time.elapsed_time()/1000<<"\n\n\n";	//divide by 1000
	//print_lists(G, leda_V1, leda_V2, leda_is_bipar, "LEDA's");
	
	if(my_is_bipar) std::cout<<"\tGraph is bipartite\n";
	else std::cout<<"\tGraph is not bipartite\n\n";
	//------------------------------------------------------------------------------------
	assert(my_is_bipar == leda_is_bipar); //same result
	
}