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