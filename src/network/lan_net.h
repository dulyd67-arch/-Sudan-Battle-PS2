#ifndef NETWORK_LAN_NET_H
#define NETWORK_LAN_NET_H
#include "../common.h"
int lan_net_init(void);
void lan_net_broadcast_state(const Actor* player, u32 seq);
#endif
