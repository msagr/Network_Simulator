#ifndef __LAYER2__
#define __LAYER2__

#include "../net.h"
#include "../gluethread/glthread.h"

#pragma pack (push,1)
typedef struct arp_hdr_{

    short hw_type;          /*1 for ethernet cable*/
    short proto_type;       /*0x0800 for IPV4*/
    char hw_addr_len;       /*6 for MAC*/
    char proto_addr_len;    /*4 for IPV4*/
    short op_code;          /*req or reply*/
    mac_add_t src_mac;      /*MAC of OIF interface*/
    unsigned int src_ip;    /*IP of OIF*/
    mac_add_t dst_mac;      /*?*/
    unsigned int dst_ip;        /*IP for which ARP is being resolved*/
} arp_hdr_t;

typedef struct ethernet_hdr_{

    mac_add_t dst_mac;
    mac_add_t src_mac;
    unsigned short type;
    char payload[248];  /*Max allowed 1500*/
    unsigned int FCS;
} ethernet_hdr_t;
#pragma pack(pop)

static inline bool_t
l2_frame_recv_qualify_on_interface(interface_t *interface, ethernet_hdr_t *ethernet_hdr) {

  if(!IS_INTF_L3_MODE(interface))
    return FALSE;
  
  /* Return TRUE if receiving machine must accept the frame */
  if(memcmp(IF_PAC(interface), ethernet_hdr->dst_mac.mac, sizeof(mac_add_t) == 0)) {
    return TRUE;
  }

  if(IS_MAC_BROADCAST_ADDR(ethernet_hdr->dst_mac.mac)) {
    return TRUE;
  }

  return FALSE;
}

/*ARP Table APIs*/
typedef struct arp_table_{

    glthread_t arp_entries;
} arp_table_t;

struct arp_entry_{

    ip_add_t ip_addr;   /*key*/
    mac_add_t mac_addr;
    char oif_name[IF_NAME_SIZE];
    glthread_t arp_glue;
    bool_t is_sane;
    /* List of packets which are pending for
     * this ARP resolution*/
    glthread_t arp_pending_list;
} arp_utility_L;
GLTHREAD_TO_STRUCT(arp_glue_to_arp_entry, arp_entry_t, arp_glue);

void
init_arp_table(arp_table_t **arp_table);

arp_entry_t *
arp_table_lookup(arp_table_t *arp_table, char *ip_addr);

void
clear_arp_table(arp_table_t *arp_table);

void
delete_arp_entry(arp_entry_t *arp_entry);

void
delete_arp_table_entry(arp_table_t *arp_table, char *ip_addr);

bool_t
arp_table_entry_add(arp_table_t *arp_table, arp_entry_t *arp_entry,
                        glthread_t **arp_pending_list);

void
dump_arp_table(arp_table_t *arp_table);

void
arp_table_update_from_arp_reply(arp_table_t *arp_table,
                                arp_hdr_t *arp_hdr, interface_t *iif);

#endif