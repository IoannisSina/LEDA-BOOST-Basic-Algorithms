#include "LEDA/graph/graph.h"
#include "LEDA/core/random_source.h"
#include "LEDA/core/list.h"
#include "LEDA/core/array.h"
#include "LEDA/system/timer.h"
#include "LEDA/graph/node_matrix.h"
#include "LEDA/graph/basic_graph_alg.h"



#include <iostream>
#include <string>
using namespace leda;

// structs and typedefs

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



// declaration of algorithm functions
void MakeEdge(graph& G, node v, node w, node_array<list<node>>& Adjacent, node_matrix<my_struct>& Index);
void RemoveEdge(graph& G, node v, node w, node_array<list<node>>& Adjacent, node_matrix<my_struct>& Index);
void MakeClosure(graph& G, node v, node w, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);
void RemoveClosure(graph& G, node v, node w, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);

void InsertEdge(graph& G, node a, node b, node_array<list<node>>& Adjacent, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);
void DeleteEdge(graph& G, node a, node b, node_array<list<node>>& Adjacent, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);


// declaration of helper functions
void add_nodes_to_graph(graph& G, int num_of_nodes); 			// adds num_of_nodes nodes in G
bool check_if_edge_exists(graph& G, node v, node w); 			// checks if a specific edge exists
edge find_edge(graph& G, node v, node w);						// finds and returns a specific edge of G given two nodes

bool edge_can_be_inserted(graph& G, node src, node trg);		// returns true if edge is ok
void tc_edges(graph& G, node_array<list<node>>& Reaches);		// prints the transitive closure edges of G

int main() {
	
	
	
	int num_of_nodes = 1000;
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
	
	
	//-----------------------------------------------INSERTING EDGES-----------------------------------------------
	
	int num_of_edges_inserted = 3;
	
	while(num_of_edges_inserted > 0) {
		
		
		node src = G.choose_node();
		node trg = G.choose_node();
		
		while(!edge_can_be_inserted(G, src, trg)) {
			src = G.choose_node();
			trg = G.choose_node();
		}
		
		InsertEdge(G, src, trg, Adjacent, Reaches, Index);
		num_of_edges_inserted--;
		
		
	}
	
	std::cout<<"Number of edges after insertions: "<<G.number_of_edges()<<"\n";
	//std::cout<<"Length of source node Adjacent after MakeEdge: "<<Adjacent[a].length()<<"\n";
	// std::cout<<"Length of target node Reaches after MakeClosure: "<<Reaches[b].length()<<"\n\n\n";
	
	//-------------------------------------------------------------------------------------------------------------
	
	node_array<int> ord(G);
	assert(TOPSORT(G, ord) == true); // assure graph is acyclic
	
	
	tc_edges(G, Reaches);
	//-----------------------------------------------DELETING EDGES------------------------------------------------
	
	// delete all edges previously inserted
	list<edge> edges_to_delete = G.all_edges();
	edges_to_delete.permute(); // randomness
	while(!edges_to_delete.empty()) {
		edge to_delete = edges_to_delete.pop();
		DeleteEdge(G, source(to_delete), target(to_delete), Adjacent, Reaches, Index);
	}
	std::cout<<"Number of edges after deletions: "<<G.number_of_edges()<<"\n";
	
	//-------------------------------------------------------------------------------------------------------------
	
	
	
	//-----------------------------------Test MakeEdge and RemoveEdge-----------------------------------
	// MakeEdge(G, a, b, Adjacent, Index);
	// std::cout<<"Number of edges after MakeEdge: "<<G.number_of_edges()<<"\n";
	// std::cout<<"Length of source node Adjacent after MakeEdge: "<<Adjacent[a].length()<<"\n\n\n";
	
	// RemoveEdge(G, a, b, Adjacent, Index);
	// std::cout<<"Number of edges after RemoveEdge: "<<G.number_of_edges()<<"\n";
	// std::cout<<"Length of source node Adjacent after RemoveEdge: "<<Adjacent[a].length()<<"\n\n\n";
	//--------------------------------------------------------------------------------------------------
	
	//--------------------------------Test MakeClosure and RemoveClosure---------------------------------
	// MakeClosure(G, a, b, Reaches, Index);
	// std::cout<<"Length of target node Reaches after MakeClosure: "<<Reaches[b].length()<<"\n\n\n";
	
	// RemoveClosure(G, a, b, Reaches, Index);
	// std::cout<<"Length of target node Reaches after RemoveClosure: "<<Reaches[b].length()<<"\n\n\n";
	//---------------------------------------------------------------------------------------------------
	
	

	
	
}
// algorithm functions
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
	
	Reaches[w].push(v);							// add v to Reaches(w)
	Index[v][w].closureSource = v;				// closure Source of index[v,w] points to v
}


void RemoveClosure(graph& G, node v, node w, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index) {
	
	Reaches[w].remove(v);				// remove v from Reaches(w)
	Index[v][w].closureSource = NULL;	// set closure Source of index[v,w] to NULL
}


void InsertEdge(graph& G, node a, node b, node_array<list<node>>& Adjacent, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index) {
	
	assert(check_if_edge_exists(G, a, b) == false); // assure that edge does not already exist
	
	
	list<std::pair<node, node>> worklist; // worklist will contain the new transitive closure edges. List of pair nodes
	node x,y,z;
	
	
	
	if(Index[a][b].refcount == 0) { 	// then <a,b> is a new transitive closure edge 
		
		MakeClosure(G, a, b, Reaches, Index);
		worklist.push(std::make_pair(a,b));
	}
	
	MakeEdge(G, a, b, Adjacent, Index);	// in any case, <a,b> is a new graph edge 
	Index[a][b].refcount += 1;			// so increment its refcount by 1
	
	
	forall(x, Reaches[a]) {				// examine each existing tc edge <x,a> 
	
		if(Index[x][b].refcount == 0) {
			
			MakeClosure(G, x, b, Reaches, Index);
			worklist.push(std::make_pair(x,b));
			
		}
		Index[x][b].refcount += 1;		// in any case, its refcount is incremented due to the new triple <x,a,b> in ref(x,b) 
		
	}
	
	
	while(!worklist.empty()) {			// examine each new tc edge <x,y> 
		
		std::pair<node, node> new_tc_edge = worklist.pop();
		x = new_tc_edge.first;
		y = new_tc_edge.second;
		
		forall(z, Adjacent[y]) {					// examine each graph edge <y,z> 
			
			if(Index[x][z].refcount == 0) {			// then <x,z> is a new transitive closure edge	

				MakeClosure(G, x, z, Reaches, Index);
				worklist.push(std::make_pair(x,z));
			}
			Index[x][z].refcount += 1;				// in any case, its refcount is incremented due to the new triple <x,y,z> in ref(x,z) 
		}
	}
	
	
}


void DeleteEdge(graph& G, node a, node b, node_array<list<node>>& Adjacent, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index) {
	
	assert(check_if_edge_exists(G, a, b) == true); // assure that edge already exists
	
	
	list<std::pair<node, node>> worklist; 	// worklist will contain the deleted transitive closure edges. List of pair nodes
	node x,y,z;
	
	RemoveEdge(G, a, b, Adjacent, Index);	// <a,b> is no longer a graph edge
	Index[a][b].refcount -= 1;				// so decrement its refcount by 1
	
	if(Index[a][b].refcount == 0) {			// then <a,b> is no longer atc edge 
	
		
		RemoveClosure(G, a, b, Reaches, Index);
		worklist.push(std::make_pair(a,b));
		
	}
	
	forall(x, Reaches[a]) {		 	// examine each tc edge <x,a> 

		
		Index[x][b].refcount -= 1;	//  decrement Index[x,b].refcount due to the triple <x,a,b> no longer in ref(x,h)
		if(Index[x][b].refcount == 0) {
			
			RemoveClosure(G, x, b, Reaches, Index);
			worklist.push(std::make_pair(x,b));
		}
	}
	
	
	while(!worklist.empty()) {		// examine each deleted tc edge <x,y>
	
		std::pair<node, node> deleted_tc_edge = worklist.pop();
		x = deleted_tc_edge.first;
		y = deleted_tc_edge.second;
		
		forall(z, Adjacent[y]) {		// examine each graph edge <y,z> 
		
			Index[x][z].refcount -= 1;	// decrement Index[x,z].refcount due to the triple <x,y,z> no longer in ref(x,z)
			if(Index[x][z].refcount == 0) {
				
				RemoveClosure(G, x, z, Reaches, Index);
				worklist.push(std::make_pair(x,z));
			}
			
		}
		
	}
	
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


bool edge_can_be_inserted(graph& G, node src, node trg) {
		
		node_array<int> ord(G);									// node_array for TOPSORT
		
		bool self_loop = (src == trg); 							// avoid self loops
		bool edge_exists = check_if_edge_exists(G, src, trg);	// assure that edge does not already exist
		
							
		edge temp_edge = G.new_edge(src, trg);					// add candidate edge
		bool g_is_acyclic = TOPSORT(G, ord);					// assure that edge will not create cycle so the DeleteEdge works after
		G.del_edge(temp_edge);									// delete candidate edge
		
		
			
		
		return !self_loop && !edge_exists && g_is_acyclic;
	
}


void tc_edges(graph& G, node_array<list<node>>& Reaches) {
	
	std::cout<<"\n\nTransitive closure edges of Graph:\n\n";
	node x, y;			// node to iterate
	
	forall(x, G.all_nodes()) {
		
		forall(y, Reaches[x]) {
			
			std::cout<<'"';
			G.print_node(y);
			std::cout<<'"';
			std::cout<<"->";
			std::cout<<'"';
			G.print_node(x);
			std::cout<<'"';
			std::cout<<"\n";
		}
		
	}
	std::cout<<"\n\n\n";
	
}