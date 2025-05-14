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

## Network APIs

1. Declare in net.h, define in net.c, use in testapp.c

  a. bool_t node_set_loopback_address(node_t *node, char *ip_addr);

  b. bool_t node_set_intf_ip_address(node_t *node, char *local_if, char *ip_addr, char mask);

  c. bool_t node_unset_intf_ip_address(node_t *node, char *local_if);

2. As soon as we add a link to the topology connecting two nodes, the end interfaces must be assigned some auto generated mac addresses.

  MAC_address generated - fn(node_name, interface_name, some heuristics)

  **used** :

  void interface_assign_mac_address(interface_t *interface);

  Display function - Display the entire network topology with networking properties also.

  **used**:

  void dump_nw_graph(graph_t *graph);

  ## Setting up Network Topology

  1. Enhance build_first_topo() in topologies.c to add networking parameyters to the graph.

  2. Display function - Display the entire network topology with networking properties also net.h/net.c

    void dump_nw_graph(graph_t *graph);
  
  3. Further we add more networking properties to the nodes and interfaces. We shall be defining more new members to node_nw_prop_t and inf_nw_props_t structures accordingly.

  ## CLI Integration

  1. User Configures/Interact with routing devices through CLI.

  **NOTE** - Check for Juniper Actual Router.

  2. Need an external CLI library using which we can implement our own customize show.config clear commands.

  3. We will use CLI to reconfigure network topology, display info. etc.

  ## Packet Processing Criteria

  1. Whenever a Routing Device receives a packet on its local interface, the first thing it has to decide is whether it should process the incoming packet or reject it right away before packet could even enter into TCP/IP stack.

  2. Acceptance or Rejection of the packet depends on many factors including but not limited to:

    a. Interface Operating Modes.
    b. Interface Configuration.
    c. Packet Contents.
  
  3. If the packet is Accepted, Routing device handover the packet to TCP/IP stack, and the ingress journey of the packet commences.
  
  ## API: Layer2/layer2.h

  Now, in position to write an API which decides whether routing device should accept or reject the incoming packet arrived on an interface operating in L3 mode:

  static inline bool_t
  l2_frame_recv_qualify_on_interface(interface_t *interface, ethernet_hdr_t *ethernet_hdr);

  - Returns TRUE, if packet should be accepted for further processing.

  - Returns FALSE, if packet should be rejected.

    ### Pseudocode :

    1. If interface is not working in L3 mode -> Return FALSE.

    2. If interface is operating in L3 mode and dst mac in ethernet hdr -- IF_MAC(interface) -> Return TRUE.

    3. If interface is operating in L3 mode and dst mac in ethernet hdr is BROADCAST MAC -> Return TRUE.

    4. Return FALSE in any other case.