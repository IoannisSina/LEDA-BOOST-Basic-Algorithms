#include "LEDA/graph/graph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/core/list.h"
#include "LEDA/core/array.h"
#include "LEDA/system/timer.h"
#include "LEDA/graph/node_matrix.h"


#include <iostream>
#include <string>
using namespace leda;

// structs

typedef struct my_struct {
	
	node edgeTarget;
	node closureSource;
	int refcount;
	
	my_struct(){
		edgeTarget = NULL;
		closureSource = NULL;
		refcount = 0;
	}
} my_struct;



// declaration of needed functions
void MakeEdge(graph& G, node v, node w, node_array<list<node>>& Adjacent, node_matrix<my_struct>& Index);
void RemoveEdge(graph& G, node v, node w, node_array<list<node>>& Adjacent, node_matrix<my_struct>& Index);
void MakeClosure(graph& G, node v, node w, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);
void RemoveClosure(graph& G, node v, node w, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);


// declaration of helper functions
void add_nodes_to_graph(graph& G, int num_of_nodes); 	// adds num_of_nodes nodes in G
bool check_if_edge_exists(graph& G, node v, node w); 	// checks if a specific edge exists
edge find_edge(graph& G, node v, node w);				// finds and returns a specific edge of G given two nodes


int main() {
	
	
	int num_of_nodes = 100;
	graph G;								// create digraph G
	add_nodes_to_graph(G, num_of_nodes);	// add nodes to G
	
	//------Test nodes------
	node a = G.new_node();
	node b = G.new_node();
	//----------------------
	
	
	
	//-----initialize needed variables-------
	node_array<list<node>> Adjacent(G);
	node_array<list<node>> Reaches(G);
	node_matrix<my_struct> Index(G);
	//---------------------------------------
	


	//-----------------------------------Test MakeEdge and RemoveEdge-----------------------------------
	MakeEdge(G, a, b, Adjacent, Index);
	std::cout<<"Number of edges after MakeEdge: "<<G.number_of_edges()<<"\n";
	std::cout<<"Length of source node Adjacent after MakeEdge: "<<Adjacent[a].length()<<"\n\n\n";
	
	RemoveEdge(G, a, b, Adjacent, Index);
	std::cout<<"Number of edges after RemoveEdge: "<<G.number_of_edges()<<"\n";
	std::cout<<"Length of source node Adjacent after RemoveEdge: "<<Adjacent[a].length()<<"\n\n\n";
	//--------------------------------------------------------------------------------------------------
	
	//--------------------------------Test MakeClosure and RemoveClosure---------------------------------
	MakeClosure(G, a, b, Reaches, Index);
	std::cout<<"Length of target node Reaches after MakeClosure: "<<Reaches[b].length()<<"\n\n\n";
	
	RemoveClosure(G, a, b, Reaches, Index);
	std::cout<<"Length of target node Reaches after RemoveClosure: "<<Reaches[b].length()<<"\n\n\n";
	//---------------------------------------------------------------------------------------------------
	
	

	
	
}
// needed functions
void MakeEdge(graph& G, node v, node w, node_array<list<node>>& Adjacent, node_matrix<my_struct>& Index) {
	
	assert(check_if_edge_exists(G, v, w) == false); // assure that edge does not already exist
	
	G.new_edge(v, w);			// add edge to graph
	Adjacent[v].push(w);		// add w to Adjacent(v)
	Index[v][w].edgeTarget = w;	// edge target of index[v,w] points to w
	
}


void RemoveEdge(graph& G, node v, node w, node_array<list<node>>& Adjacent, node_matrix<my_struct>& Index) {
	
	assert(check_if_edge_exists(G, v, w) == true); // assure that edge exists in order to remove it
	
	//---------find edge-------------------
	edge to_delete_edge = find_edge(G, v, w);
	//-------------------------------------
	
	G.del_edge(to_delete_edge);		// remove edge from graph
	Adjacent[v].remove(w);			// remove w from Adjacent(v)
	Index[v][w].edgeTarget = NULL;	// set edgeTarget to NULL
	
}


void MakeClosure(graph& G, node v, node w, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index) {
	
	Reaches[w].push(v);				// add v to Reaches(w)
	Index[v][w].closureSource = v;	// closure Source of index[v,w] points to v
}


void RemoveClosure(graph& G, node v, node w, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index) {
	
	Reaches[w].remove(v);				// remove v from Reaches(w)
	Index[v][w].closureSource = NULL;	// set closure Source of index[v,w] to NULL
}
// helper functions
bool check_if_edge_exists(graph& G, node v, node w) {
	
	edge e;
	forall_edges(e, G) {
		
		node src = source(e);
		node trg = target(e);
		
		if((src == v) && (trg == w))
			return true;
	}
	return false;
	
}


void add_nodes_to_graph(graph& G, int num_of_nodes) {
	
	for(int i=0; i < num_of_nodes; i++)
		G.new_node();
}


edge find_edge(graph& G, node v, node w) {
	
	assert(check_if_edge_exists(G, v, w) == true); // assure that edge exists
	
	edge e;
	forall_edges(e, G) {
		
		node src = source(e);
		node trg = target(e);
		
		if((src == v) && (trg == w))
			return e;
	}
	
	
}
