

void
init_arp_table(arp_table_t **arp_table){

    *arp_table = calloc(1, sizeof(arp_table_t));
    init_glthread(&((*arp_table)->arp_entries));
}

arp_entry_t *
arp_table_lookup(arp_table_t *arp_table, char *ip_addr){

    glthread_t *curr;
    arp_entry_t *arp_entry;
    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr){
    
        arp_entry = arp_glue_to_arp_entry(curr);
        if(strncmp(arp_entry->ip_addr.ip_addr, ip_addr, 16) == 0){
            return arp_entry;
        }
    } ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr);
    return NULL;
}

void
clear_arp_table(arp_table_t *arp_table){

    glthread_t *curr;
    arp_entry_t *arp_entry;

    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr){
        
        arp_entry = arp_glue_to_arp_entry(curr);
        delete_arp_entry(arp_entry);
    } ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr);
}

void
delete_arp_table_entry(arp_table_t *arp_table, char *ip_addr){

    arp_entry_t *arp_entry = arp_table_lookup(arp_table, ip_addr);
    
    if(!arp_entry)
        return;

    delete_arp_entry(arp_entry);
}

bool_t
arp_table_entry_add(arp_table_t *arp_table, arp_entry_t *arp_entry,
                    glthread_t **arp_pending_list){

    if(arp_pending_list){
        assert(*arp_pending_list == NULL);   
    }

    arp_entry_t *arp_entry_old = arp_table_lookup(arp_table, 
            arp_entry->ip_addr.ip_addr);

    /* Case 0 : if ARP table do not exist already, then add it
     * and return TRUE*/
    if(!arp_entry_old){
        glthread_add_next(&arp_table->arp_entries, &arp_entry->arp_glue);
        return TRUE;
    }
    

    /*Case 1 : If existing and new ARP entries are full and equal, then
     * do nothing*/
    if(arp_entry_old &&
            IS_ARP_ENTRIES_EQUAL(arp_entry_old, arp_entry)){

        return FALSE;
    }

    /*Case 2 : If there already exists full ARP table entry, then replace it*/
    if(arp_entry_old && !arp_entry_sane(arp_entry_old)){
        delete_arp_entry(arp_entry_old);
        init_glthread(&arp_entry->arp_glue);
        glthread_add_next(&arp_table->arp_entries, &arp_entry->arp_glue);
        return TRUE;
    }

    /*Case 3 : if existing ARP table entry is sane, and new one is also
     * sane, then move the pending arp list from new to old one and return FALSE*/
    if(arp_entry_old &&
        arp_entry_sane(arp_entry_old) &&
        arp_entry_sane(arp_entry)){
    
        if(!IS_GLTHREAD_LIST_EMPTY(&arp_entry->arp_pending_list)){
            glthread_add_next(&arp_entry_old->arp_pending_list,
                    arp_entry->arp_pending_list.right);
        }
        if(arp_pending_list)
            *arp_pending_list = &arp_entry_old->arp_pending_list;
        return FALSE;
    }

    /*Case 4 : If existing ARP table entry is sane, but new one if full,
     * then copy contents of new ARP entry to old one, return FALSE*/
    if(arp_entry_old && 
        arp_entry_sane(arp_entry_old) && 
        !arp_entry_sane(arp_entry)){

        strncpy(arp_entry_old->mac_addr.mac, arp_entry->mac_addr.mac, sizeof(mac_add_t));
        strncpy(arp_entry_old->oif_name, arp_entry->oif_name, IF_NAME_SIZE);
        arp_entry_old->oif_name[IF_NAME_SIZE -1] = '\0';

        if(arp_pending_list)
            *arp_pending_list = &arp_entry_old->arp_pending_list;
        return FALSE;
    }

    return FALSE;
}

void
dump_arp_table(arp_table_t *arp_table){

    glthread_t *curr;
    arp_entry_t *arp_entry;

    ITERATE_GLTHREAD_BEGIN(&arp_table->arp_entries, curr){

        arp_entry = arp_glue_to_arp_entry(curr);
        printf("IP : %s, MAC : %u:%u:%u:%u:%u:%u, OIF = %s, Is Sane : %s\n", 
            arp_entry->ip_addr.ip_addr, 
            arp_entry->mac_addr.mac[0], 
            arp_entry->mac_addr.mac[1], 
            arp_entry->mac_addr.mac[2], 
            arp_entry->mac_addr.mac[3], 
            arp_entry->mac_addr.mac[4], 
            arp_entry->mac_addr.mac[5], 
            arp_entry->oif_name,
            arp_entry_sane(arp_entry) ? "TRUE" : "FALSE");
    } ITERATE_GLTHREAD_END(&arp_table->arp_entries, curr);
}

void
arp_table_update_from_arp_reply(arp_table_t *arp_table, 
                                arp_hdr_t *arp_hdr, interface_t *iif){

    unsigned int src_ip = 0;
    glthread_t *arp_pending_list = NULL;

    assert(arp_hdr->op_code == ARP_REPLY);

    arp_entry_t *arp_entry = calloc(1, sizeof(arp_entry_t));

    src_ip = htonl(arp_hdr->src_ip);

    inet_ntop(AF_INET, &src_ip, arp_entry->ip_addr.ip_addr, 16);

    arp_entry->ip_addr.ip_addr[15] = '\0';

    memcpy(arp_entry->mac_addr.mac, arp_hdr->src_mac.mac, sizeof(mac_add_t));

    strncpy(arp_entry->oif_name, iif->if_name, IF_NAME_SIZE);

    arp_entry->is_sane = FALSE;

    bool_t rc = arp_table_entry_add(arp_table, arp_entry, &arp_pending_list);

    glthread_t *curr;
    arp_pending_entry_t *arp_pending_entry;

    if(arp_pending_list){
        
        ITERATE_GLTHREAD_BEGIN(arp_pending_list, curr){
        
            arp_pending_entry = arp_pending_entry_glue_to_arp_pending_entry(curr);

            remove_glthread(&arp_pending_entry->arp_pending_entry_glue);

            process_arp_pending_entry(iif->att_node, iif, arp_entry, arp_pending_entry);
            
            delete_arp_pending_entry(arp_pending_entry);

        } ITERATE_GLTHREAD_END(arp_pending_list, curr);

        (arp_pending_list_to_arp_entry(arp_pending_list))->is_sane = FALSE;
    }

    if(rc == FALSE){
        delete_arp_entry(arp_entry);
    }
}