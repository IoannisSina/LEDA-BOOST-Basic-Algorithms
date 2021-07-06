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

// Enum support graph vertices
enum nodeType { closure, join };

// structs
typedef struct support_struct {
	
	list<node> label;
	nodeType type;
	
} support_struct;


typedef struct my_struct {
	
	node edgeTarget;
	node closureSource;
	int refcount;
	int SGnode;
	
	my_struct(){
		edgeTarget = NULL;
		closureSource = NULL;
		refcount = 0;
		SGnode = -1;	//int which shows the index of support node in support list
	}
} my_struct;






// declaration of algorithm functions
void MakeEdge(graph& G, node v, node w, node_array<list<node>>& Adjacent, node_matrix<my_struct>& Index);
void RemoveEdge(graph& G, node v, node w, node_array<list<node>>& Adjacent, node_matrix<my_struct>& Index);
void MakeClosure(graph& G, node v, node w, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);
void RemoveClosure(graph& G, node v, node w, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);

void InsertEdge(graph& G, GRAPH<int, edge>& supportG, list<support_struct>& support_list, node a, node b, node_array<list<node>>& Adjacent, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);
void DeleteEdge(graph& G, node a, node b, node_array<list<node>>& Adjacent, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);

//find path operations
void new_tc_edge_insertion(graph& G, GRAPH<int, edge>& supportG, list<support_struct>& support_list, node x, node z, node_matrix<my_struct>& Index);
void refcount_incremented_insertion(graph& G, GRAPH<int, edge>& supportG, list<support_struct>& support_list, node x, node y, node z, node_matrix<my_struct>& Index);
void findpath(graph& G, node x, node z, list<support_struct>& support_list, node_matrix<my_struct>& Index);


// declaration of helper functions
void add_nodes_to_graph(graph& G, int num_of_nodes); 			// adds num_of_nodes nodes in G
bool check_if_edge_exists(graph& G, node v, node w); 			// checks if a specific edge exists
edge find_edge(graph& G, node v, node w);						// finds and returns a specific edge of G given two nodes

bool edge_can_be_inserted(graph& G, node src, node trg);		// returns true if edge is ok
void tc_edges(graph& G, node_array<list<node>>& Reaches);		// prints the transitive closure edges of G
node find_predecessor(node x, node z, list<support_struct>& support_list); // return a predecessor of <x,y>
node find_node_by_info(GRAPH<int, edge>& supportG, int info);	// returns a node from support G by info
std::pair<node,node> custom_path(int length, graph& G, GRAPH<int, edge>& supportG, list<support_struct>& support_list, node a, node b, node_array<list<node>>& Adjacent, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index);



int main() {
	
	
	int num_of_nodes = 1000;
	graph G;								// create digraph G
	
	list<support_struct> support_list;		// contains structs for each node
	GRAPH<int, edge> supportG;				// create support Graph	
	
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
	
	int num_of_edges_inserted = 10;
	
	while(num_of_edges_inserted > 0) {
		
		
		node src = G.choose_node();
		node trg = G.choose_node();
		
		while(!edge_can_be_inserted(G, src, trg)) {
			src = G.choose_node();
			trg = G.choose_node();
		}
		
		InsertEdge(G, supportG, support_list, src, trg, Adjacent, Reaches, Index);
		num_of_edges_inserted--;
		
		
	}
	
	std::cout<<"Number of edges after insertions: "<<G.number_of_edges()<<"\n";
	// std::cout<<"Length of source node Adjacent after MakeEdge: "<<Adjacent[a].length()<<"\n";
	// std::cout<<"Length of target node Reaches after MakeClosure: "<<Reaches[b].length()<<"\n\n\n";
	
	//--------------------------------------------------TESTS------------------------------------------------------
	
	
	node_array<int> ord(G);
	assert(TOPSORT(G, ord) == true); // assure graph is acyclic
	
	
	std::pair<node,node> my_pair = custom_path(11, G, supportG, support_list, a, b, Adjacent, Reaches, Index);
	// InsertEdge(G, supportG, support_list, first, prev, Adjacent, Reaches, Index);	// Base of recursion for find path
	std::cout<<"\n\nPath returned from findpath:\n\n";
	findpath(G, my_pair.first, my_pair.second, support_list, Index);
	std::cout<<"\n\n";
	
	std::cout<<"Length of support list: "<<support_list.length()<<"\n";
	//tc_edges(G, Reaches);
	
	//-------------------------------------------------------------------------------------------------------------
	
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


void InsertEdge(graph& G, GRAPH<int, edge>& supportG, list<support_struct>& support_list, node a, node b, node_array<list<node>>& Adjacent, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index) {
	
	assert(check_if_edge_exists(G, a, b) == false); // assure that edge does not already exist
	
	
	list<std::pair<node, node>> worklist; // worklist will contain the new transitive closure edges. List of pair nodes
	node x,y,z;
	
	
	
	if(Index[a][b].refcount == 0) { 	// then <a,b> is a new transitive closure edge 
		
		MakeClosure(G, a, b, Reaches, Index);
		worklist.push(std::make_pair(a,b));
		new_tc_edge_insertion(G, supportG, support_list, a, b, Index); // find path operation for new tc edge
		
		
	}
	
	MakeEdge(G, a, b, Adjacent, Index);	// in any case, <a,b> is a new graph edge 
	Index[a][b].refcount += 1;			// so increment its refcount by 1
	
	
	forall(x, Reaches[a]) {				// examine each existing tc edge <x,a> 
	
		if(Index[x][b].refcount == 0) {
			
			MakeClosure(G, x, b, Reaches, Index);
			worklist.push(std::make_pair(x,b));
			new_tc_edge_insertion(G, supportG, support_list, x, b, Index); // find path operation for new tc edge
			
		}
		Index[x][b].refcount += 1;		// in any case, its refcount is incremented due to the new triple <x,a,b> in ref(x,b)
		refcount_incremented_insertion(G, supportG, support_list, x, a, b, Index); // find path operation
		
	}
	
	
	while(!worklist.empty()) {			// examine each new tc edge <x,y> 
		

		std::pair<node, node> new_tc_edge = worklist.pop();
		x = new_tc_edge.first;
		y = new_tc_edge.second;
		
		forall(z, Adjacent[y]) {					// examine each graph edge <y,z> 
			
			if(Index[x][z].refcount == 0) {			// then <x,z> is a new transitive closure edge	

				MakeClosure(G, x, z, Reaches, Index);
				worklist.push(std::make_pair(x,z));
				new_tc_edge_insertion(G, supportG, support_list, x, z, Index); // find path operation for new tc edge
			}
			Index[x][z].refcount += 1;				// in any case, its refcount is incremented due to the new triple <x,y,z> in ref(x,z)
			refcount_incremented_insertion(G, supportG, support_list, x, y, z, Index); // find path operation			
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


// find path operations
void new_tc_edge_insertion(graph& G, GRAPH<int, edge>& supportG, list<support_struct>& support_list, node x, node z, node_matrix<my_struct>& Index) {
	
	//---------------------------find path operation for new tc edge--------------------------------------
		support_struct new_sup_node;				// create support struct node
		new_sup_node.type = closure;				// it is a closure node
		new_sup_node.label.push_back(x);			// so push a,b to the list
		new_sup_node.label.push_back(z);	
		
		support_list.push_back(new_sup_node); 		// push new node to support_list
		
		int correct_index = support_list.length()-1;// node in support graph will kepp this index to know its index in support_list
		supportG.new_node(correct_index);			// create new node for support G with correct index
		
		Index[x][z].SGnode = correct_index;			// set SGnode to point to this closure node
	//-----------------------------------------------------------------------------------------------------
}


void refcount_incremented_insertion(graph& G, GRAPH<int, edge>& supportG, list<support_struct>& support_list, node x, node y, node z, node_matrix<my_struct>& Index){
	
	//---------------------------find path operation for new tc edge--------------------------------------
	support_struct new_sup_node;							// create support struct node
	new_sup_node.type = join;								// it is a join node
	new_sup_node.label.push_back(x);						// so push x,y,z to the list
	new_sup_node.label.push_back(y);	
	new_sup_node.label.push_back(z);
	
	support_list.push_back(new_sup_node); 					// push new node to support_list
	int correct_index = support_list.length()-1;			// node in support graph will kepp this index to know its index in support_list
	node new_join_node = supportG.new_node(correct_index);	// create new node for support G with correct index
	
	
	// create edge from node labeled <x, y> and <y, z> to new join vertex
	int x_y_closure_index = Index[x][y].SGnode; // node labeled <x,y> index in support_list
	int y_z_closure_index = Index[y][z].SGnode;	// node labeled <y,z> index in support_list
	
	
	assert(support_list.contents(support_list.get_item(x_y_closure_index)).type == closure);
	assert(support_list.contents(support_list.get_item(y_z_closure_index)).type == closure);
	
	node x_y_node = find_node_by_info(supportG, x_y_closure_index);	// get node <x,y> from supportG
	node y_z_node = find_node_by_info(supportG, y_z_closure_index);	// get node <y,z> from supportG
	
	supportG.new_edge(x_y_node, new_join_node);
	supportG.new_edge(y_z_node, new_join_node);
	
	// create edge from new join vertex to node labeled <x,z>
	int x_z_closure_index = Index[x][z].SGnode; // node labeled <x,z> index in support_list
	assert(support_list.contents(support_list.get_item(x_z_closure_index)).type == closure);
	node x_z_node = find_node_by_info(supportG, x_z_closure_index);	// get node <x,z> from supportG
	supportG.new_edge(new_join_node, x_z_node);
	//----------------------------------------------------------------------------------------------------
}


void findpath(graph& G, node x, node z, list<support_struct>& support_list, node_matrix<my_struct>& Index) {
	
	
	if(Index[x][z].edgeTarget != NULL) {		// BASE case
	
		G.print_node(x);
		std::cout<<"\n";
		G.print_node(z);
		std::cout<<"\n";
		//std::cout<<"Base of recursion\n";
		
	} else {									// recursive step
	
		//find a predecessor of <x,z> in support_list
		node y = find_predecessor(x, z, support_list);
		findpath(G, x, y, support_list, Index);
		G.print_node(z);
		std::cout<<"\n";
		//std::cout<<"Recursive step\n";
	
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


node find_predecessor(node x, node z, list<support_struct>& support_list) {
	
	support_struct j;
	
	forall(j, support_list) {
		
		if(j.type == join) { //check only join nodes
		
			list<node> label = j.label;
			
			node first_node = label.contents(label.get_item(0));
			node third_node = label.contents(label.get_item(2));
			
			if((first_node == x) && (third_node == z))
				return label.contents(label.get_item(1));	// return from <x,y,z> y
			
		}
		
	}
	std::cout<<"SOMETHING IS WRONG!!\n";
	
}


node find_node_by_info(GRAPH<int, edge>& supportG, int info) {
	
	node x;
	forall(x, supportG.all_nodes()) {
		
		if(supportG.inf(x) == info)
			return x;
	}
}


std::pair<node,node> custom_path(int length, graph& G, GRAPH<int, edge>& supportG, list<support_struct>& support_list, node a, node b, node_array<list<node>>& Adjacent, node_array<list<node>>& Reaches, node_matrix<my_struct>& Index) {
	
	std::cout<<"\n\nActual path:\n\n";
	node first = G.choose_node();
	node prev = first;
	
	G.print_node(first);
	std::cout<<"\n";
	for(int i=0 ;i < 11; i++) {
		
		node next = G.choose_node();
		while(!edge_can_be_inserted(G, prev, next)) 
			next = G.choose_node();
			
		InsertEdge(G, supportG, support_list, prev, next, Adjacent, Reaches, Index);
		G.print_node(next);
		std::cout<<"\n";
		prev = next;
	}
	std::cout<<"\n\n";
	return std::make_pair(first, prev);
}