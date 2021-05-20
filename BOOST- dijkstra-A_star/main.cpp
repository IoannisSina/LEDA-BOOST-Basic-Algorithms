#include <boost/graph/adjacency_list.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/random.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <queue>
#include <string>
#include <time.h>
#include <math.h>
#include <vector>


using namespace boost;

struct NodeInfo;
struct EdgeInfo;

typedef boost::adjacency_list<vecS,vecS,directedS,NodeInfo,EdgeInfo> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_d;
typedef boost::graph_traits<Graph>::edge_descriptor edge_d;

struct NodeInfo{
	
	vertex_d pred;				//Pointer to predicate vertex
	double distance;			//Etiketa of node
	double heuristic;			//value of heuristic for node
	
	NodeInfo(){
		distance = INT_MAX;
		pred = graph_traits<Graph>::null_vertex();
	}
	
};

struct EdgeInfo{
	double weight;
	
	EdgeInfo(){
		weight = 1 + (rand() % 100);	//Weight..	
	}
};



property_map<Graph, vertex_d NodeInfo::*>::type v_pred;						//pred array
property_map<Graph, double NodeInfo::*>::type v_dist_dijkstra;				//distance array for dijkstra. MAX_INT means node is never reached
//property_map<Graph, int NodeInfo::*>::type v_dist_A_star;					//distance array for dijkstra

//functions
void dijkstra_SP(Graph& g, vertex_d starting_node, vertex_d ending_node, bool SP_version, bool print_checked_nodes);
void A_star(Graph& g, vertex_d starting_node, vertex_d ending_node);
void ht_for_grid(Graph& g, int r, int c, vertex_d ending_node);
void ht_for_random_graph(Graph& g, vertex_d ending_node);
std::pair<vertex_d, vertex_d> random_grid_node(Graph& g, int r, int c);

//helper functions
Graph reverse_graphs(const Graph& g);
bool dijkstra_ckecker(const Graph& g);
void print_graph_info(const Graph& g, vertex_d starting_node, vertex_d ending_node);

//graphs
void grid_graph(Graph& g, int r, int c);

//Times functions
void grid_graph_timings();
void random_graph_timings();


int main() {
	
		
	//timings for edge wight from 1 to 100. It can be changed in the EdgeInfo struct
	
	grid_graph_timings();
	std::cout<<"\n\n\n";
	random_graph_timings();
	
	//--------------------------------------------------CHECK CHECKER--------------------------------------------------
	//check if dijkstra_ckecker is working correclty
	// Graph check_checker;
	// grid_graph(check_checker, 30, 1000);
	// std::pair<vertex_d, vertex_d> rand_nodes = random_grid_node(check_checker, 30, 1000);
	// starting_node = rand_nodes.first;
	// ending_node = rand_nodes.second;
	// dijkstra_SP(check_checker, starting_node, ending_node, false, false);
	// assert(dijkstra_ckecker(check_checker) == true);
	// std::cout<<"Dijkstra_Sp works fine!!\n";
	//print_graph_info(check_checker, starting_node, ending_node);
	//-----------------------------------------------------------------------------------------------------------------
	

}

void dijkstra_SP(Graph& g, vertex_d starting_node, vertex_d ending_node, bool SP_version, bool print_checked_nodes){
	
	
	std::vector< std::pair <vertex_d, double> > dist_vec;
	v_pred = get (&NodeInfo::pred, g);													//get pred for all nodes 
	v_dist_dijkstra = get (&NodeInfo::distance, g);										//get distances for all nodes
	v_dist_dijkstra[starting_node] = 0;													//set starting node distance to 0
	
	
	property_map<Graph, double EdgeInfo::*>::type e_weight = get (&EdgeInfo::weight, g);
	
	//---------bool array to mark checked nodes--------------------
	int g_num_nodes = num_vertices(g);
	bool v_checked[g_num_nodes];
	std::fill_n(v_checked, g_num_nodes, false);
	//-------------------------------------------------------------
	
	graph_traits<Graph>::vertex_iterator vi,vi_end;
	for(tie(vi,vi_end)=vertices(g);vi!=vi_end;vi++) dist_vec.push_back(std::make_pair(*vi, g[*vi].distance));
	int number_of_nodes_checked = 0;
	//main loop
	while(!dist_vec.empty()){
		
		//------------------------------------------------------
		//std::sort(dist_vec.begin(), dist_vec.end(), sortbysec);
		std::pair <vertex_d, int> min_dist = dist_vec[0];
		int min_index = 0;
		for(int k=0; k < dist_vec.size(); k++) {
			
			if(dist_vec[k].second < min_dist.second){
				
				min_dist = dist_vec[k];
				min_index = k;
			}
		}
		vertex_d min_dist_v = min_dist.first;
		dist_vec.erase(dist_vec.begin() + min_index);
		number_of_nodes_checked++;
		//------------------------------------------------------
		//std::cout<<"hello "<<min_dist_v<<"\n";				//check if it stops when we pop ending node
		if(SP_version && (min_dist_v == ending_node)) break;	//SP..if popped node is the ending node then break;
		
		graph_traits < Graph >::out_edge_iterator ei, ei_end;	//iterators for agjac edges of node
		
		for(tie(ei, ei_end) = out_edges(min_dist_v, g); ei != ei_end; ei++) {
			//std::cout<<*ei;
			vertex_d target = boost::target(*ei, g );			//target of adj edge
			
			if(v_checked[target] == false){						//check only if node in Q
				
				double temp_dist = v_dist_dijkstra[min_dist_v] + e_weight[*ei];
				if (temp_dist < v_dist_dijkstra[target]){		//relaxation 
					
					v_dist_dijkstra[target] = temp_dist;
					v_pred[target] = min_dist_v;
					//--------fix distances----------------
					for(int h=0; h < dist_vec.size(); h++){
						
						if(dist_vec[h].first == target){
							
							dist_vec[h].second = temp_dist;
							break;
						}
					}
					//-------------------------------------
				}
			}
			
			
		}
		
		v_checked[min_dist_v] = true;							//checked node
		
	}
	if(print_checked_nodes) std::cout<<" number of nodes checked is "<<number_of_nodes_checked<<std::endl;
}

void A_star(Graph& g, vertex_d starting_node, vertex_d ending_node){
	
	//----------------------calculate new costs for edges----------------------
	
	graph_traits<Graph>::edge_iterator ei, ei_end;
	for(tie(ei,ei_end)=edges(g); ei!=ei_end;ei++){
		
		vertex_d source = boost::source(*ei, g);
		vertex_d target = boost::target(*ei, g);
		
		g[*ei].weight += g[target].heuristic - g[source].heuristic; 
	}
	
	//--------------------------------------------------------------------------
	//call dijkstra_SP
	dijkstra_SP(g, starting_node, ending_node, true, true);
	
}

void ht_for_grid(Graph& g, int r, int c, vertex_d ending_node){
	
	//we calculate heuristics for all nodes from ending_node and keep then in the struct
	int num_of_nodes = r * c;
	//calculate row, column of ending node. we know it is at the last column
	int ending_node_row = ending_node - ((c-1)*r);		//for the row we subtract the first id of the column from the ending_node id
	int ending_node_col = c-1;
	
	//calculate heuristic for all nodes based on the euclidean distanse
	int curr_node_id;
	for(int i=0; i < c; i++){
		
		for(int j=0; j < r; j++){
			
			curr_node_id = i*r + j;						//we move from top to bottom and from lrfto to right. The way we built the grid graph
			
			double dist = pow((j - ending_node_row), 2) + pow((i - ending_node_col), 2);
			g[vertex(curr_node_id, g)].heuristic = sqrt(dist);
			
		}
	}
	
	
	
	
}

void ht_for_random_graph(Graph& g, vertex_d ending_node){
	
	
	//---------------------------------------------------------------------------
	Graph copy_g_L1_to_all, copy_g_all_to_L1, copy_g_L2_to_all, copy_g_all_to_L2;	//4 copy graphs to keep distances of each dijkstra
	copy_g_L1_to_all = g;
	copy_g_all_to_L1 = reverse_graphs(g);			//reverse g. So with dijkstra we find distances from all nodes to L1
	copy_g_L2_to_all = g;
	copy_g_all_to_L2 = reverse_graphs(g);			//reverse g. So with dijkstra we find distances from all nodes to L2
	//---------------------------------------------------------------------------
	//random generator
	mt19937 rng;
	rng.seed(uint32_t(time(0)));
	uniform_int<> u(0,1000);
	variate_generator<mt19937&, uniform_int<> > rng1(rng,u);
	
	//--------------------------------------L1--------------------------------------
	
	vertex_d L1 = random_vertex(copy_g_L1_to_all, rng);
	//while(L1 == ending_node) L1 = random_vertex(copy_g_L1_to_all, rng); //avoid having same L1 and ending node
	
	
	dijkstra_SP(copy_g_L1_to_all, L1, ending_node, false, false);		//Dijkstra from L1 to all nodes
	//assert(dijkstra_ckecker(copy_g_L1_to_all) == true);					//assure that algorithm runs correctly
	//print_graph_info(copy_g_L1_to_all, L1, ending_node);
	
	dijkstra_SP(copy_g_all_to_L1, L1, ending_node, false, false);		//Dijkstra from all nodes to L1. After execution d(i) will be shortest distance from i to L1
	//assert(dijkstra_ckecker(copy_g_all_to_L1) == true);					//assure that algorithm runs correctly
	//print_graph_info(copy_g_all_to_L1, L1, ending_node);
	//------------------------------------------------------------------------------
	
	//-----------------------------------Find L2------------------------------------ L2 must be the node with the biggest distance from L1
	graph_traits<Graph>::vertex_iterator vi, vi_end;
	int max_distance = INT_MIN;
	vertex_d L2 = graph_traits<Graph>::null_vertex();
	for(tie(vi,vi_end)=vertices(copy_g_L1_to_all); vi!=vi_end;vi++){
		
		if(copy_g_L1_to_all[*vi].distance > max_distance) {
			
			L2 = *vi;
			max_distance = copy_g_L1_to_all[*vi].distance;
			
		}
	}
	//------------------------------------------------------------------------------
	//std::cout<<L2<<std::endl;
	
	//--------------------------------------L2--------------------------------------
	
	dijkstra_SP(copy_g_L2_to_all, L2, ending_node, false, false);		//Dijkstra from L1 to all nodes
	//assert(dijkstra_ckecker(copy_g_L2_to_all) == true);					//assure that algorithm runs correctly
	//print_graph_info(copy_g_L2_to_all, L2, ending_node);
	
	dijkstra_SP(copy_g_all_to_L2, L2, ending_node, false, false);		//Dijkstra from all nodes to L1. After execution d(i) will be shortest distance from i to L1
	//assert(dijkstra_ckecker(copy_g_all_to_L2) == true);					//assure that algorithm runs correctly
	//print_graph_info(copy_g_all_to_L2, L2, ending_node);
	//------------------------------------------------------------------------------
	
	//-------------------------set heuristics to initial g--------------------------
	//for every node i we calculate d(L1,t)- d(L1,i) and d(i,L1) - D(t,L1) and keep the maximum max1
	//for every node i we calculate d(L2,t)- d(L2,i) and d(i,L2) - D(t,L2) and keep the maximum max2
	//we assign the maximum of max1,max2 to node i
	int L1_to_i, i_to_L1, L2_to_i, i_to_L2;
	int max1, max2;
	for(tie(vi,vi_end)=vertices(g); vi!=vi_end;vi++){
		
		//L1
		L1_to_i = copy_g_L1_to_all[ending_node].distance - copy_g_L1_to_all[*vi].distance;	//d(L1,t)- d(L1,i)
		i_to_L1 = copy_g_all_to_L1[*vi].distance - copy_g_all_to_L1[ending_node].distance;	//d(i,L1) - D(t,L1)
		
		max1 = std::max(L1_to_i, i_to_L1);
		
		//L2
		L2_to_i = copy_g_L2_to_all[ending_node].distance - copy_g_L2_to_all[*vi].distance;	//d(L2,t)- d(L2,i)
		i_to_L2 = copy_g_all_to_L2[*vi].distance - copy_g_all_to_L2[ending_node].distance;	//d(i,L2) - D(t,L2)
		
		max2 = std::max(L2_to_i, i_to_L2);
		
		//std::cout<<max1<<" "<<max2<<"\n";
		
		g[*vi].heuristic = std::max(max1, max2);
	}
	
	//------------------------------------------------------------------------------
	
}

std::pair<vertex_d, vertex_d> random_grid_node(Graph& g, int r, int c){
	
	 int num_of_nodes = r * c;
	/* initialize random seed: */
	srand (time(NULL));
	
	//choose starting node from first column
	//The graph is built in a way that node 0 to r-1 will be the nodes posisioned in the first column. So we generate a random int from 0 to r-1
	int starting_node = rand() % (r-1);

	//for the ending node we have to choose from last column. Last column will start from (c-1) * r + 1 and will end at num_of_nodes-1
	int last_column_start = (c-1) * r;
	int ending_node = last_column_start + (rand() % (r-1));
	
	return std::make_pair(vertex(starting_node, g), vertex(ending_node, g));
}

void grid_graph(Graph& g, int r, int c){
	
	edge_d normal_edge;
	edge_d reverse_edge;
	//we add the reverse edges so the graph is bigraph
	
	vertex_d curr_row[r];						//create an array of capacity c in order to save the current row and connect it to the next one
	
	//---------------first column-------------------------
	vertex_d current_node = add_vertex(g);		//node 0,0
	curr_row[0] = current_node;
	for(int j=1; j < r; j++){					//first column
			
			vertex_d new_node = add_vertex(g);
			curr_row[j] = new_node;				//keep first node
			//----------------------------------
			normal_edge = add_edge(current_node, new_node, g).first;
			reverse_edge = add_edge(new_node, current_node, g).first;
			
			g[reverse_edge].weight = g[normal_edge].weight; //normal and reverse edges must have same weight
			//----------------------------------
			current_node = new_node;
	}
	// for(int h=0;h<r;h++){
		
		// std::cout<<curr_row[h]<<"\n";
	// }
	//----------------------------------------------------
	
	//-------------rest of the columns--------------------
	for(int i=0; i < c-1; i++){
		
		vertex_d current_node = add_vertex(g);		//iteration node 0,2...
		//--------------------------------------
		normal_edge = add_edge(curr_row[0], current_node, g).first;
		reverse_edge = add_edge(current_node, curr_row[0], g).first;
		
		g[reverse_edge].weight = g[normal_edge].weight; //normal and reverse edges must have same weight
		//--------------------------------------------------
		curr_row[0] = current_node;
		
		
		for(int j=1; j < r; j++){					//first column
			
			vertex_d new_node = add_vertex(g);
			//-------------------------------------------------------
			normal_edge = add_edge(curr_row[j], new_node, g).first;		//add edge from column i to i+1
			reverse_edge = add_edge(new_node, curr_row[j], g).first;
			
			g[reverse_edge].weight = g[normal_edge].weight; 					//normal and reverse edges must have same weight
			//-------------------------------------------------------
			curr_row[j] = new_node;												//keep new node in array for next iter
			//------------------------------------------------
			normal_edge = add_edge(current_node, new_node, g).first;
			reverse_edge = add_edge(new_node, current_node, g).first;
			
			g[reverse_edge].weight = g[normal_edge].weight; 					//normal and reverse edges must have same weight
			//------------------------------------------------
			current_node = new_node;
		}
	}
	
	
	
	
}

Graph reverse_graphs(const Graph& g){
	
	Graph reversed_g = g;
	std::vector< std::pair <vertex_d, vertex_d> > reverse_edges;
	//remove all edges from reversed_g
	graph_traits<Graph>::edge_iterator ei, ei_end;
	for(tie(ei,ei_end)=edges(g); ei!=ei_end;ei++){
		
		vertex_d source = boost::source(*ei, reversed_g);
		vertex_d target = boost::target(*ei, reversed_g);
		remove_edge(source, target, reversed_g);
	}
	
	
	for(tie(ei,ei_end)=edges(g); ei!=ei_end;ei++){
		
		vertex_d source = boost::source(*ei, g);
		vertex_d target = boost::target(*ei, g);
		edge_d new_edge = add_edge(target, source, reversed_g).first;
		reversed_g[new_edge].weight = g[*ei].weight;
		
	}
	//std::cout<<num_edges(reversed_g)<<"\n";
	return reversed_g;
}
    
bool dijkstra_ckecker(const Graph& g){
	
	int num_of_nodes = num_vertices(g);					//check if the equality is true for at leaste n-1 nodes
	bool is_ok = true;
	int num_of_equalities = 0;
	graph_traits<Graph>::edge_iterator ei, ei_end;
	for(tie(ei,ei_end)=edges(g); ei!=ei_end;ei++){
		
		vertex_d source = boost::source(*ei, g);
		vertex_d target = boost::target(*ei, g);
		
		 
		if(v_dist_dijkstra[target] <= v_dist_dijkstra[source] + g[*ei].weight){
			
			if(v_dist_dijkstra[target] == v_dist_dijkstra[source] + g[*ei].weight) num_of_equalities++;
		}else is_ok == true;
		
	}
	
	return (is_ok && (num_of_equalities >= num_of_nodes-1));
	
}

void print_graph_info(const Graph& g, vertex_d starting_node, vertex_d ending_node){
	
	
	std::cout<<"Starting node: "<<starting_node<<std::endl;
	std::cout<<"Ending node: "<<ending_node<<"\n\n\n";
	graph_traits<Graph>::edge_iterator ei, ei_end;
	for(tie(ei,ei_end)=edges(g); ei!=ei_end;ei++){
		
		std::cout<<"\t"<<*ei<<" with cost: "<<g[*ei].weight<<"\n";
	}
	
	
	
	//print distances
	std::cout<<"\n\n\n";
	graph_traits<Graph>::vertex_iterator vi, vi_end;
	for(tie(vi,vi_end)=vertices(g); vi!=vi_end;vi++){
		
		//----------------check that property map works fine----------------
		
		// if((v_dist_dijkstra[*vi] != g[*vi].distance) || (v_pred[*vi] != g[*vi].pred)) {
			
			// std::cout<<"FAIL!!\n";
			// break;
		// }
		
		//------------------------------------------------------------------
		std::cout<<"Node: "<<*vi<<" with heuristic value: "<<g[*vi].heuristic<<" and distance: "<<g[*vi].distance<<" and pred: ";
		if(g[*vi].pred == graph_traits<Graph>::null_vertex()) std::cout<<"has no pred\n";
		else std::cout<<g[*vi].pred<<"\n";
	}
	
}

void grid_graph_timings(){
	
	//--------------------------------------------GRID GRAPH AND HEURISTICS-------------------------------------------------------------------
	
	//------------Set number of nodes and edges------------
	std::vector< std::pair <int, int> > grid_parameters;
	grid_parameters.push_back(std::make_pair(30, 1000));
	grid_parameters.push_back(std::make_pair(60, 1000));
	grid_parameters.push_back(std::make_pair(80, 1000));
	//-----------------------------------------------------
	//----------------TIMES--------------------------------
	boost::posix_time::ptime t1, t2;
    boost::posix_time::time_duration diff_dijkstra, diff_astar;
	//-----------------------------------------------------
	for(int i=0; i < grid_parameters.size(); i++){
	
		int r = grid_parameters[i].first, c = grid_parameters[i].second;
		std::cout<<"\nNumber of nodes is "<<grid_parameters[i].first<<" and number of edges is "<<grid_parameters[i].second<<"\n\n";
		Graph g_grid, g_dijkstra_grid, g_astar_grid;
		vertex_d starting_node_grid, ending_node_grid;
		
		grid_graph(g_grid, r, c);														//create grid graph
		std::pair<vertex_d, vertex_d> rand_nodes = random_grid_node(g_grid, r, c);		//set starting_node andf ending_node
		starting_node_grid = rand_nodes.first;											//keep starting_node	
		ending_node_grid = rand_nodes.second;											//kepp ending_node
		
		g_dijkstra_grid = g_grid;														//copy initial graph for dijkstra
		g_astar_grid = g_grid;															//copy initial graph for Astar
		
		std::cout<<"\tFor dijkstra_SP the";
		
		t1 = boost::posix_time::microsec_clock::local_time();							//start time for dijkstra_SP
		dijkstra_SP(g_dijkstra_grid, starting_node_grid, ending_node_grid, true, true);	//run dijkstra_SP for graph
		t2 = boost::posix_time::microsec_clock::local_time();							//end time for dijkstra_SP
		diff_dijkstra = t2 - t1;														//time for dijkstra_SP
		
		
		//Astar
		std::cout<<"\tFor A star the";
		ht_for_grid(g_astar_grid, r, c, ending_node_grid);								//calculate heuristics of grid graph. Preprocessing
		t1 = boost::posix_time::microsec_clock::local_time();							//start time for A_star
		A_star(g_astar_grid, starting_node_grid, ending_node_grid);						//run A_star for graph
		t2 = boost::posix_time::microsec_clock::local_time();							//end time for A_star
		diff_astar = t2 - t1;															//time for dijkstra_SP
		
		std::cout<<"\n\tDistance for ending_node dijkstra: "<<g_dijkstra_grid[ending_node_grid].distance<<"\n";
		std::cout<<"\tDistance for ending_node A_star: "<<g_astar_grid[ending_node_grid].distance + g_astar_grid[starting_node_grid].heuristic<<"\n";
		std::cout <<"\tTime for dijkstra_SP: "<<diff_dijkstra.total_seconds()<<" seconds"<<"\n";
		std::cout <<"\tTime for A star: "<<diff_astar.total_seconds()<<" seconds"<<"\n";
		//print_graph_info(g_dijkstra, starting_node, ending_node);
		//print_graph_info(g_astar, starting_node, ending_node);
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------
	
}

void random_graph_timings(){
	
	//---------------------------------------------------RANDOM GRAPH-------------------------------------------------------------------------
	
	//------------Random generator-------------------------
	mt19937 rng;
	rng.seed(uint32_t(time(0)));
	uniform_int<> u(0,1000);
	variate_generator<mt19937&, uniform_int<> > rng1(rng,u);
	//-----------------------------------------------------
	//------------Set number of nodes and edges------------
	std::vector< std::pair <int, int> > random_graph_parameters;
	random_graph_parameters.push_back(std::make_pair(10000, 20000));
	random_graph_parameters.push_back(std::make_pair(20000, 40000));
	random_graph_parameters.push_back(std::make_pair(60000, 120000));
	//-----------------------------------------------------
	//----------------TIMES--------------------------------
	boost::posix_time::ptime t1, t2;
    boost::posix_time::time_duration diff_dijkstra, diff_astar;
	//-----------------------------------------------------
	
		for(int i=0; i < random_graph_parameters.size(); i++){
		int num_nodes = random_graph_parameters[i].first, num_edges = random_graph_parameters[i].second;
		std::cout<<"\nNumber of nodes is "<<random_graph_parameters[i].first<<" and number of edges is "<<random_graph_parameters[i].second<<"\n\n";
		
		Graph g_random, g_dijkstra_random, g_astar_random;
		vertex_d starting_node_random, ending_node_random;
		
		generate_random_graph(g_random, num_nodes, num_edges, rng, false , false);														//create random graph
			

		srand (time(NULL));																												//seed
		starting_node_random = vertex(rand()% num_vertices(g_random), g_random);														//set starting_node andf ending_node
		ending_node_random = vertex(rand()% num_vertices(g_random), g_random);															//random_vertex(g, rng);
		while(starting_node_random == ending_node_random) ending_node_random = vertex(rand()% num_vertices(g_random), g_random);		//avoid having same starting and ending nodes

		
		g_dijkstra_random = g_random;																									//copy initial graph for dijkstra
		g_astar_random = g_random;																										//copy initial graph for Astar
		
		
		std::cout<<"\tFor dijkstra_SP the";
		
		t1 = boost::posix_time::microsec_clock::local_time();																			//start time for dijkstra_SP
		dijkstra_SP(g_dijkstra_random, starting_node_random, ending_node_random, true, true);											//run dijkstra_SP for graph
		t2 = boost::posix_time::microsec_clock::local_time();																			//end time for dijkstra_SP
		diff_dijkstra = t2 - t1;																										//time for dijkstra_SP
		
		
		//Astar
		std::cout<<"\tFor A star the";
		
		ht_for_random_graph(g_astar_random, ending_node_random);																		//calculate heuristics of grid graph. Preprocessing
		t1 = boost::posix_time::microsec_clock::local_time();																			//start time for A_star
		A_star(g_astar_random, starting_node_random, ending_node_random);																//run A_star for graph
		t2 = boost::posix_time::microsec_clock::local_time();																			//end time for A_star
		diff_astar = t2 - t1;																											//time for dijkstra_SP
		
		
		std::cout<<"\n\tDistance for ending_node dijkstra: "<<g_dijkstra_random[ending_node_random].distance<<"\n";
		std::cout<<"\tDistance for ending_node A_star: "<<g_astar_random[ending_node_random].distance + g_astar_random[starting_node_random].heuristic<<"\n";
		std::cout <<"\tTime for dijkstra_SP: "<<diff_dijkstra.total_milliseconds()<<" milliseconds"<<"\n";
		std::cout <<"\tTime for A star: "<<diff_astar.total_milliseconds()<<" milliseconds"<<"\n";
		if(g_dijkstra_random[ending_node_random].pred == graph_traits<Graph>::null_vertex()) std::cout<<"\tEnding node not discovered\n";
		else std::cout<<"\tPred of ending node: "<<g_dijkstra_random[ending_node_random].pred<<"\n";
		//print_graph_info(g_dijkstra, starting_node, ending_node);
		//print_graph_info(g_astar, starting_node, ending_node);
	}
	//----------------------------------------------------------------------------------------------------------------------------------------------
	
}

