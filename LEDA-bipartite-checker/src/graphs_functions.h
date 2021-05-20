#pragma once
using namespace leda;

//declarations

void first_family(ugraph& G, int num_of_nodes);
void second_family(ugraph& G, int num_of_nodes);
void third_family(ugraph& G, int k);
void fourth_family(ugraph& G, int k);



//functions

//nested squares n nodes and 2*n-4 edges
void first_family(ugraph& G, int num_of_nodes){
	
	if(num_of_nodes%4 != 0) std::cout<<"Number of nodes must be divisible by 4\n"; //check if number divides 4
	else{
		
		int num_of_squares = num_of_nodes/4;
		//create first square and then create num_of_nodes - 1 more squares
		node prev_up_left = G.new_node();			//upper left corner node
		node prev_up_right = G.new_node();			//upper right corner node
		node prev_down_left = G.new_node();			//down left corner node
		node prev_down_right = G.new_node();		//down right corner node
		
		//connect the nodes
		G.new_edge(prev_up_left, prev_up_right);		
		G.new_edge(prev_up_right, prev_down_right);		
		G.new_edge(prev_down_right, prev_down_left);		
		G.new_edge(prev_down_left, prev_up_left);
		
		for(int j=0; j < num_of_squares - 1; j++){	//-1 squares because we intialize the first one
			
			//create new nodes for next square
			node next_up_left = G.new_node();		
			node next_up_right = G.new_node();		
			node next_down_left = G.new_node();		
			node next_down_right = G.new_node();
			
			//connect next nodes (new) with old square's nodes (prev)
			G.new_edge(prev_up_left, next_up_left);		
			G.new_edge(prev_up_right, next_up_right);		
			G.new_edge(prev_down_left, next_down_left);		
			G.new_edge(prev_down_right, next_down_right);
			
			//connect new nodes 
			G.new_edge(next_up_left, next_up_right);		
			G.new_edge(next_up_right, next_down_right);		
			G.new_edge(next_down_right, next_down_left);		
			G.new_edge(next_down_left, next_up_left);
			
			//make prev nodes equal to next ones
			prev_up_left = next_up_left;
			prev_up_right = next_up_right;
			prev_down_left = next_down_left;
			prev_down_right = next_down_right;
		}
		
	}
}

//daktylios n nodes and n edges 
void second_family(ugraph& G, int num_of_nodes){
	
	
	node first = G.new_node();		//new node
	node prev = first;				//keep first node to connect it with the last one
	node next;
	
	for(int i=0; i < num_of_nodes-1; i++){
		
		next = G.new_node();
		G.new_edge(prev, next);		//new edge
		prev = next;				//make prev next
	}
	G.new_edge(prev, first);		//connect last node with first one
	
	
}

//2k edges between levels
void third_family(ugraph& G, int k){
	
	//create an array for every level of length K 
	array<node> L1(k);
	array<node> L2(k);
	array<node> L3(k);
	array<node> L4(k);
	//---------------------------edges from each node to its corresponing in the next leve (i.e straight line)-------------
	for(int i=0; i < k; i++){				//create the first k edges for each level to the other
		
		node new_L1 = G.new_node();
		node new_L2 = G.new_node();
		node new_L3 = G.new_node();
		node new_L4 = G.new_node();
		L1[i] = new_L1;
		L2[i] = new_L2;
		L3[i] = new_L3;
		L4[i] = new_L4;
		
		G.new_edge(new_L1, new_L2); 	//connect the newest node to the next leve's  newer
		G.new_edge(new_L2, new_L3);		//connect the newest node to the next leve's  newer
		G.new_edge(new_L3, new_L4);		//connect the newest node to the next leve's  newer
	}
	//-------------------------edges from one random node to all of the next leve's nodes)-------------
	
	random_source s(0, k-1);									//Pick random node from 0 to k-1
	int random_index;											//random index
	
						/*k-1 edges from random node level 1 to level 2*/
	random_index = s();
	node random_node = L1[random_index];
	
	for(int j=0; j < k; j++){
		
		if(j != random_index) G.new_edge(random_node, L2[j]);	//avoid creating an existing edge
	}
	
	
						/*k-1 edges from random node level 2 to level 3*/
	random_index = s();
	random_node = L2[random_index];
	
	for(int j=0; j < k; j++){
		
		if(j != random_index) G.new_edge(random_node, L3[j]);	//avoid creating an existing edge
	}
	
	
						/*k-1 edges from random node level 3 to level 4*/
	random_index = s();
	random_node = L3[random_index];
	
	for(int j=0; j < k; j++){
		
		if(j != random_index) G.new_edge(random_node, L4[j]);	//avoid creating an existing edge
	}
	
	
	//-------------------------2 edges from level 1 to 3 and from 2 to 4-------------------------------
	G.new_edge(L1[s()], L3[s()]);
	G.new_edge(L2[s()], L4[s()]);
	
}

//my graph (bonus)
void fourth_family(ugraph& G, int k){
	
	//create nodes x,y,u,w
	node x = G.new_node();
	node y = G.new_node();
	node u = G.new_node();
	node w = G.new_node();
	
	//create four edges
	G.new_edge(x, u);
	G.new_edge(x, w);
	G.new_edge(y, u);
	G.new_edge(y, w);
	
	//create first k nodes connected to x,y
	for(int i=0; i < k; i++){
		
		node temp1 = G.new_node();
		G.new_edge(x, temp1);
		G.new_edge(y, temp1);
	}
	
	//create more k nodes connected to u,w
	for(int j=0; j < k; j++){
		
		node temp2 = G.new_node();
		G.new_edge(u, temp2);
		G.new_edge(w, temp2);
	}
}
