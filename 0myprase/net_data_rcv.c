
网络接收数据流程

net\core\dev.c
static __latent_entropy void net_rx_action(struct softirq_action *h)
    n = list_first_entry(&list, struct napi_struct, poll_list);
    budget -= napi_poll(n, &repoll);
    
net\core\dev.c
static int napi_poll(struct napi_struct *n, struct list_head *repoll)
    if (test_bit(NAPI_STATE_SCHED, &n->state)) {
        work = n->poll(n, weight);  --> igb_poll()
        trace_napi_poll(n, work, weight);
    }

drivers\net\ethernet\intel\igb\igb_main.c
// igb_poll - NAPI Rx polling callback
static int igb_poll(struct napi_struct *napi, int budget) 
    if (q_vector->rx.ring) {
        int cleaned = igb_clean_rx_irq(q_vector, budget);
        
drivers\net\ethernet\intel\igb\igb_main.c
static int igb_clean_rx_irq(struct igb_q_vector *q_vector, const int budget)
    while (likely(total_packets < budget)) {
        ...
        /* retrieve a buffer from the ring  从环中检索缓冲区 */
        skb = igb_fetch_rx_buffer(rx_ring, rx_desc, skb);
        if (igb_is_non_eop(rx_ring, rx_desc))
            continue;
        if (igb_cleanup_headers(rx_ring, rx_desc, skb)) {
            skb = NULL;
            continue;
        }
        /* populate checksum, timestamp, VLAN, and protocol */
        igb_process_skb_fields(rx_ring, rx_desc, skb);
        napi_gro_receive(&q_vector->napi, skb);

net\core\dev.c
gro_result_t napi_gro_receive(struct napi_struct *napi, struct sk_buff *skb)
    skb_gro_reset_offset(skb);
    return napi_skb_finish(dev_gro_receive(napi, skb), skb);
    
static int netif_receive_skb_internal(struct sk_buff *skb)
    ret = __netif_receive_skb(skb);
        ret = __netif_receive_skb_core(skb, false);
        
net\core\dev.c
static int __netif_receive_skb_core(struct sk_buff *skb, bool pfmemalloc)
// pcap逻辑，这里会将数据送入抓包点，tcpdump就是从这个入口获取的
     list_for_each_entry_rcu(ptype, &ptype_all, list) {  
          if (pt_prev)
            ret = deliver_skb(skb, pt_prev, orig_dev);
        pt_prev = ptype;
        }

static inline int deliver_skb(struct sk_buff *skb, struct packet_type *pt_prev, 
                              struct net_device *orig_dev)
    return pt_prev->func(skb, skb->dev, pt_prev, orig_dev);  // arp_rcv() 或者 ip_rcv() 

net\ipv4\ip_input.c
int ip_rcv(struct sk_buff *skb, struct net_device *dev, 
           struct packet_type *pt, struct net_device *orig_dev)
    return NF_HOOK(NFPROTO_IPV4, NF_INET_PRE_ROUTING, net, NULL, skb, dev, NULL, ip_rcv_finish);
    
static int ip_rcv_finish(struct net *net, struct sock *sk, struct sk_buff *skb)
    int err = ip_route_input_noref(skb, iph->daddr, iph->saddr, iph->tos, dev);
    return dst_input(skb);
        return skb_dst(skb)->input(skb);  //include\net\dst.h    --> ip_local_deliver()

net\ipv4\ip_input.c
int ip_local_deliver(struct sk_buff *skb)
    return NF_HOOK(NFPROTO_IPV4, NF_INET_LOCAL_IN, net, NULL, skb, 
           skb->dev, NULL, ip_local_deliver_finish);
   
net\ipv4\ip_input.c
static int ip_local_deliver_finish(struct net *net, struct sock *sk, struct sk_buff *skb)
     int protocol = ip_hdr(skb)->protocol;
     const struct net_protocol *ipprot;  
     ipprot = rcu_dereference(inet_protos[protocol]);
    ret = ipprot->handler(skb);  // tcp_v4_rcv() 或者 udp_rcv()