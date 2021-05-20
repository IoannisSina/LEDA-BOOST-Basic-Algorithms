#pragma once
using namespace boost;

//declarations
void print_lists(const graph& G, const list<node>& V1, const list<node>& V2, bool is_bipar, string name);
bool my_bipar_checker(const ugraph& G, list<node>& V1, list<node>& V2);
list<node> my_BFS(const ugraph& G, node s, node_array<int>& dist, node_array<edge>& pred, node_array<char>& color_of_node);