#include "gluethread/glthread.h"

#define NODE_NAME_SIZE 16
#define IF_NAME_SIZE 16
#define MAX_INTF_PER_NODE 10

/* Forward Declarations */
typedef struct node_ node_t;
typedef struct link_ link_t;
typedef struct graph_ graph_t;

typedef  struct interface_ {
  char if_name[IF_NAME_SIZE];
  struct node_ *att_node;
  struct link_ *link;
} interface_t;

struct link_ {
  interface_t intf1;
  interface_t intf2;
  unsigned int cost;
};

struct node_ {
  char node_name[NODE_NAME_SIZE];
  interface_t *intf[MAX_INTF_PER_NODE];
  glthread_t graph_glue;
  unsigned int udp_port_number;
  int udp_sock_fd;
  node_nw_prop_t node_nw_prop;
};

typedef struct graph_ {
  char topology_name[32];
  glthread_t node_list;
};


