#pragma once
using namespace leda;

//declarations

void print_lists(const graph& G, const list<node>& V1, const list<node>& V2, bool is_bipar, string name);
bool my_bipar_checker(const ugraph& G, list<node>& V1, list<node>& V2);
list<node> my_BFS(const ugraph& G, node s, node_array<int>& dist, node_array<edge>& pred, node_array<char>& color_of_node);




//functions 

void print_lists(const graph& G, const list<node>& V1, const list<node>& V2, bool is_bipar, string name){
	
	node v;
	if(is_bipar){							//if bipartite print nodes
		
		std::cout<<name<<" lists: \n\t";
		forall(v, V1){
			G.print_node(v);
			std::cout<<" ";
		}
		std::cout<<"\n\t";
		
		forall(v, V2){
			G.print_node(v);
			std::cout<<" ";
		}
		std::cout<<"\n";
	}else{									//if not bipartite print circle			
		
		std::cout<<name<<" circle: \n\t";
		forall(v, V1){
			G.print_node(v);
			std::cout<<" ";
		}
		std::cout<<"\n";
	}
}

//my_bipar_checker with the addition of color node_array in it
bool my_bipar_checker(const ugraph& G, list<node>& V1, list<node>& V2){
	
	//--------------------------------------------my_BFS----------------------------------------------------------------------
	
	
	
	node_array<int> dist (G);											//level array of BFS for each node (-1 if not visited) 
	node_array<edge> pred (G); 											//pred array of BFS for each node (nil if not visited) 
	node_array<char> color_of_node (G);									//color array for each node's color in my_BFS 
	node s = G.choose_node();											//choose random node to start
	list<node> visited_nodes = my_BFS(G, s, dist, pred, color_of_node);	//call my_BFS
	assert(G.number_of_nodes() == visited_nodes.length()); 				//connected graph all nodes marked

	
	//-------------------------------------------CHECKER----------------------------------------------------------------------
	
	/*We return in the same way as LEDAS Is_Bipartite. If G is bipartite the two sides are returned in V1 and V2, respectively. 
	If G is not bipartite the node sequence of an odd-length circle is returned in V1.*/
	
	bool is_bipar = true;
	node src,trg;										//nodes
	edge e;												//edge e as iterator
	forall_edges(e,G){									//iterate through edges to check is there is an edge whose source and target have the same color
	
		src = source(e);
		trg = target(e);
		
		if(color_of_node[src] == color_of_node[trg]){	//G is not bipartite
			is_bipar = false;							//is_bipar is false
			break;										//stop iterating through edges as we know G is not bipartite
		}
		
		
	}
	
	node u;
	
	//-----------------------------------------fill V1,V2 if G is bipartite else fill V1 wih odd circle nodes-----
	
	if(is_bipar){														//G is bipartite
		
		forall_nodes(u,G){												//green nodes in V1 and blue in V2
			
			(color_of_node[u] == 'b') ? V1.append(u) : V2.append(u); 	//blue nodes in V1 and green ones in V2
		}
	}else{																//G is not bipartite. Follow path from pred list until u find the same node
	
		/*The odd circle will be like this -> s--->x x->y y--->s
		So we find path from x to u and from y to u and merge them. u is the nearest common predicate node*/
		node src_current_node = src;
		node trg_current_node = trg;
		list<node> src_path;
		list<node> trg_path;
		/*Let (x,y) be the edge that makes our graph not bipartite. We know that its target and source will be at the same level*/
		
		//Proxwrame pros ta pisw taftoxrona kai afou ksekiname apo to idio epipedo kseroume oti tha stamatisei sto koino prokatoxo
		while(src_current_node != trg_current_node){	//go backwards until u find same node. You are sure that sometime it will be the same node
			
		
			src_path.push_front(src_current_node);
			edge pre1 = pred[src_current_node];
			src_current_node = (src_current_node == source(pre1))? target(pre1):source(pre1);
			
			
			trg_path.append(trg_current_node);
			edge pre2 = pred[trg_current_node];
			trg_current_node = (trg_current_node == source(pre2))? target(pre2):source(pre2);
			
		}		
		
		src_path.conc(trg_path);						//merge two lists
		src_path.push_front(src_current_node);			//push the nearest common predicate to list 
		V1 = src_path;
	}
	
	
	//------------------------------------------------------------------------------------------------------------
	
	return is_bipar;
	//------------------------------------------------------------------------------------------------------------------------
	
}

//my_BFS with color node_array
list<node> my_BFS(const ugraph& G, node s, node_array<int>& dist, node_array<edge>& pred, node_array<char>& color_of_node){ //we added the color node_array in the function
	
	
	queue<node> q;  				//initialize an empty queue
	list<node> visited_nodes;  		//to return list (visited_nodes)
	edge e;
	node a,b;
	
	forall_nodes(a,G){				//set every node as unmarked
		dist[a] = -1;
		pred[a] = nil;
		color_of_node[a] = 'n';		// not colored
	}
	
	
	dist[s] = 0;					//set the starting node's level to 0
	q.append(s);					//append s in queue  
	color_of_node[s] = 'g';			//S has green color even levels
	visited_nodes.append(s);		//append s in return list nodes
	
	while(!q.empty()){
		
		b = q.pop();				//pick node from queue (first time S)
		
		
		forall_adj_edges(e,b){		//for all adjacent edges of b (poped node)
			
			a = (source(e) == b) ? target(e) : source(e);//Undirected graph so we check which node is NOT b. If source of e is b then a is target of e else a is source of e
			
			if(dist[a] < 0){				//if a node is unseen
				q.append(a);				// append it to queue
				visited_nodes.append(a);	//append it to visited nodes
				pred[a] = e;				//set e as pred of a
				
				dist[a] = dist[b] + 1;								// set its level the previous node's level + 1
				color_of_node[a] = (dist[a] % 2 == 1) ? 'b' : 'g';	//if in odd level then set color to blue. else to green 
			}
		}
	}
	return visited_nodes;
}
