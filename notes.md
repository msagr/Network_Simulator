## Generic Graph Construction

1. First, we shall develop a library using which we can create static graph.

2. A graph, as we know, consists of nodes, edges, cost of edges.

3. This Graph can be used to implement for other purposes :
    a. Network Topology
    b. Routing protocols development
    c. Practice of Graph Algo. (Dijkstra)

4. As of now graph nodes are simple nodes, but now they will represent network devices.

## Graph

1. A graph is a collection of nodes.

2. An interface has a:
    a. Name
    b. Owning node
    c. A wire (or link)

3. A link is defined as pair of interfaces.

4. A node has a:
    a. Name
    b. Set of empty interface slots

**NOTE** - Try to model Data structure such that it depicts the Organization of information in real physical world.


## Graph Construction APIs

1. graph_t *create_new_graph(char *topology_name);

2. node_t *create_graph_node(graph_t* graph, char *node_name);

3. void insert_link_between_two_nodes(node_t *node1, node_t *node2, char *from_if_name, char *to_if_name, unsigned int cost);

**NOTE** - Need to extend generic graph to represent network topology. 

## Network topology

1. DS APIs related to network config shall be defined in net.h/net.c

2. Every node has its own IP Address, called as loopback address.

3. Every interface must have MAC address and may have ip address/mask.