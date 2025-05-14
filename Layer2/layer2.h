#ifndef __LAYER2__
#define __LAYER2__

#include "../net.h"
#include "../gluethread/glthread.h"

#pragma pack (push,1)
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

#endif