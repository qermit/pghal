#include <stdlib.h>
#include "pghal_inc.h"

#include "sdb_bus.h"

#include "sdb.h"
#include "wb_sdb_rom.h"


struct wb_sdb_entry {
   struct pghal_list list;
   uint32_t entry_id;
   struct sdb_empty data;
   struct wb_sdb_rom * parent;
};


static uint64_t inline ntohll(uint64_t netlonglong) {
  if (htons(1) == 1) return netlonglong;

  uint64_t res = netlonglong;
  uint32_t * tmp_res = (uint32_t *)&res;
  uint32_t * tmp_origin = (uint32_t *)&netlonglong; 
  tmp_res[0] = ntohl(tmp_origin[1]);
  tmp_res[1] = ntohl(tmp_origin[0]);

  return res;
}



static void decode_sdb_product(struct sdb_product * product){
  product->vendor_id = ntohll(product->vendor_id);
  product->device_id = ntohl(product->device_id);
  product->version   = ntohl(product->version);
  product->date      = ntohl(product->date);
//  printf("Product name: |%.19s|\n", product->name);
}

static void decode_sdb_component(struct sdb_component * component)
{
  component->addr_first =  ntohll(component->addr_first);
  component->addr_last  =  ntohll(component->addr_last);
  decode_sdb_product(&component->product);
}


static struct sdb_interconnect * decode_sdb_interconnect(struct sdb_empty * empty)
{
    struct sdb_interconnect * interconnect = (struct sdb_interconnect * ) empty;
    if (ntohl(interconnect->sdb_magic) != SDB_MAGIC) { 
       return NULL;
    }
    interconnect->sdb_magic = ntohl(interconnect->sdb_magic);
    interconnect->sdb_records = ntohs(interconnect->sdb_records);

    decode_sdb_component(&interconnect->sdb_component); 
    return interconnect;
    
}
static struct sdb_device * decode_sdb_device(struct sdb_empty * empty){
    struct sdb_device * device = (struct sdb_device * ) empty;

    device->abi_class= ntohs(device->abi_class);
    device->bus_specific= ntohl(device->bus_specific);

    decode_sdb_component(&device->sdb_component);
    return device;
}
static struct sdb_bridge * decode_sdb_bridge(struct sdb_empty * empty){
    struct sdb_bridge * bridge = (struct sdb_bridge * ) empty;
    bridge->sdb_child = ntohll(bridge->sdb_child);
    decode_sdb_component(&bridge->sdb_component);
    return bridge;
}

struct wb_sdb_rom * wb_sdb_rom_create_child(struct wb_sdb_rom * parent, struct wb_sdb_entry * sdb_entry);

int wb_sdb_rom_read_entry(struct wb_sdb_rom * sdb_rom, uint32_t entry_id, struct sdb_empty * entry)
{
  struct sdb_node_address reg_dna;

  memcpy(&reg_dna, &sdb_rom->sdb.address, sizeof(struct sdb_node_address));
  reg_dna.sdb_address = reg_dna.sdb_address + entry_id * SDB_RECORD_LENGTH;

  pghal_bus_read(sdb_rom->sdb.bus, &reg_dna.address, sizeof(struct sdb_empty), entry);
  int i;  
  for (i = 0; i<sizeof(struct sdb_empty)/sizeof(uint32_t); i++) {
     uint32_t tmp = ((uint32_t *) entry)[i];
     ((uint32_t *) entry)[i] = ntohl(tmp);
  }

  return 0;
}


struct wb_sdb_rom * wb_sdb_get_sdb_rom_by_id(struct wb_sdb_rom * sdb_rom, uint16_t id) {

   struct wb_sdb_rom * return_sdb_rom = NULL;
   struct wb_sdb_rom * cursor_sdb_rom = NULL;
   list_for_each_entry(cursor_sdb_rom, &sdb_rom->rom_list, list) {
     if (cursor_sdb_rom->entry_id == id) {
       return_sdb_rom = cursor_sdb_rom;
       break;
     }
   }
  
  return return_sdb_rom;
}

struct wb_sdb_entry * wb_sdb_get_by_id(struct wb_sdb_rom * sdb_rom, uint16_t id) {

   struct wb_sdb_entry * return_sdb_entry = NULL;
   struct wb_sdb_entry * cursor_sdb_entry = NULL;
   list_for_each_entry(cursor_sdb_entry, &sdb_rom->entry_list, list) {
     if (cursor_sdb_entry->entry_id == id) {
       return_sdb_entry = cursor_sdb_entry;
       break;
     }
   }
  
  return return_sdb_entry;
}

struct wb_sdb_entry * wb_sdb_get_by_ids(struct wb_sdb_rom * sdb_rom, char * id_string, struct wb_sdb_rom ** sdb_rom_entry)
{
  struct wb_sdb_rom   * current_level_sdb_rom = sdb_rom;
  struct wb_sdb_entry * ret_entry =  NULL;
  char * save_ptr;
  char * id_copy = strdup(id_string);
  char *token = strtok_r(id_copy, ".", &save_ptr);
  while(token != NULL && current_level_sdb_rom != NULL) {
    uint32_t entry_id = strtoul(token, NULL, 0); 
    if ( save_ptr[0] != 0) {
      printf("Trying to get SDB rom for entry_id: %d\n", entry_id);
      current_level_sdb_rom = wb_sdb_get_sdb_rom_by_id(current_level_sdb_rom, entry_id); 
    } else {
      printf("Trying to get SDB entry for entry_id: %d\n", entry_id);
      ret_entry = wb_sdb_get_by_id(current_level_sdb_rom, entry_id);
    }
    token = strtok_r(NULL, ".", &save_ptr);
  }
  
  if (sdb_rom_entry!= NULL) * sdb_rom_entry = current_level_sdb_rom;

  return ret_entry;
}

struct wb_sdb_rom * wb_sdb_get_sdb_rom_by_ids(struct wb_sdb_rom * sdb_rom, char * id_string)
{
   struct wb_sdb_rom * current_sdb_rom = NULL;
   wb_sdb_get_by_ids(sdb_rom, id_string, &current_sdb_rom);

   return current_sdb_rom;
}


struct wb_sdb_entry * wb_sdb_find_next(struct wb_sdb_rom * sdb_rom, struct sdb_empty * entry);


enum sdb_record_type wb_sdb_get_type_by_id(struct wb_sdb_rom * sdb_rom, uint16_t id) {
  struct wb_sdb_entry * ret_entry = wb_sdb_get_by_id(sdb_rom, id);
  enum sdb_record_type ret_type = sdb_type_empty;
  if (ret_entry != NULL) {
    ret_type = ret_entry->data.record_type;
  }
  return ret_type; 
}

enum sdb_record_type wb_sdb_get_type_by_ids(struct wb_sdb_rom * sdb_rom, char * id_string) {
   struct wb_sdb_entry * ret_entry = wb_sdb_get_by_ids(sdb_rom, id_string, NULL);
   enum sdb_record_type ret_type = sdb_type_empty; 
   if (ret_entry != NULL) {
      ret_type = ret_entry->data.record_type;
   }
   return ret_type;
}


int wb_sdb_get_addr_by_id(struct wb_sdb_rom * sdb_rom, uint16_t id, uint32_t * sdb_address)
{
   int ret_val = -2;
   char id_string[6];
   sprintf(id_string, "%d", id);
   struct wb_sdb_entry * entry = wb_sdb_get_by_ids(sdb_rom, id_string, NULL);
   if (entry == NULL) {
     return -1;
   } else {
     struct sdb_component *tmp_component = NULL;
     if (entry->data.record_type == sdb_type_device) tmp_component = &((struct sdb_device*) &entry->data)->sdb_component;
     if (entry->data.record_type == sdb_type_interconnect) tmp_component = &((struct sdb_interconnect*) &entry->data)->sdb_component;
     if (entry->data.record_type == sdb_type_bridge) tmp_component = &((struct sdb_bridge*) &entry->data)->sdb_component;
     if (tmp_component != NULL) {
//        printf("tmp_component%02X: %08X\n", entry->data.record_type, tmp_component->addr_first);
       *sdb_address = tmp_component->addr_first + entry->parent->bus_address;
        ret_val = 0;
     }
   }
   return ret_val;
}
// 0 -> return name of root interconnect
// 1 -> return name of device/bridge/... in root interconnect
// 1.0 -> return name of interconnect inside bridge
// 1.1 ->  return name of device/bridge/... in bridged interconnect
// returns length of name
int wb_sdb_get_addr_by_ids(struct wb_sdb_rom * sdb_rom, char * id_string, uint32_t * sdb_address)
{
   int ret_val = -2;
//   printf("wb_sdb_get_addr_by_id: %s\n", id_string);
   struct wb_sdb_entry * entry = wb_sdb_get_by_ids(sdb_rom, id_string, NULL);
   if (entry == NULL) {
     return -1;
   } else {
     struct sdb_component *tmp_component = NULL;
     if (entry->data.record_type == sdb_type_device) tmp_component = &((struct sdb_device*) &entry->data)->sdb_component;
     if (entry->data.record_type == sdb_type_interconnect) tmp_component = &((struct sdb_interconnect*) &entry->data)->sdb_component;
     if (entry->data.record_type == sdb_type_bridge) tmp_component = &((struct sdb_bridge*) &entry->data)->sdb_component;
     if (tmp_component != NULL) {
//        printf("tmp_component%02X: %08X\n", entry->data.record_type, tmp_component->addr_first);
       *sdb_address = tmp_component->addr_first + entry->parent->bus_address;
        ret_val = 0;
     }
   }
   return ret_val;
}

int wb_sdb_get_name_by_id(struct wb_sdb_rom * sdb_rom, uint16_t id, char * name)
{
   int ret_val = 0;
   char id_string[6];
   sprintf(id_string, "%d", id);
   struct wb_sdb_entry * entry = wb_sdb_get_by_ids(sdb_rom, id_string, NULL);
   if (entry == NULL) {
//     printf("Entry not found\n");
     return -1;
   } else {
     struct sdb_component *tmp_component = NULL;
     if (entry->data.record_type == sdb_type_device) tmp_component = &((struct sdb_device*) &entry->data)->sdb_component;
     if (entry->data.record_type == sdb_type_interconnect) tmp_component = &((struct sdb_interconnect*) &entry->data)->sdb_component;
     if (entry->data.record_type == sdb_type_bridge) tmp_component = &((struct sdb_bridge*) &entry->data)->sdb_component;
     if (tmp_component != NULL) {
       ret_val = sprintf(name, "%.19s", tmp_component->product.name);
     }
   }
   return ret_val;
}

int wb_sdb_get_name_by_ids(struct wb_sdb_rom * sdb_rom, char * id_string, char * name)
{
   int ret_val = 0;
   struct wb_sdb_entry * entry = wb_sdb_get_by_ids(sdb_rom, id_string, NULL);
   if (entry == NULL) {
//     printf("Entry not found\n");
     return -1;
   } else {
     struct sdb_component *tmp_component = NULL;
     if (entry->data.record_type == sdb_type_device) tmp_component = &((struct sdb_device*) &entry->data)->sdb_component;
     if (entry->data.record_type == sdb_type_interconnect) tmp_component = &((struct sdb_interconnect*) &entry->data)->sdb_component;
     if (entry->data.record_type == sdb_type_bridge) tmp_component = &((struct sdb_bridge*) &entry->data)->sdb_component;
     if (tmp_component != NULL) {
       ret_val = sprintf(name, "%.19s", tmp_component->product.name);
     }
   }
   return ret_val;

}

void wb_sdb_rom_dump(struct wb_sdb_rom * sdb_rom, int level)
{
  struct wb_sdb_entry * current_entry = NULL;
  struct wb_sdb_rom * current_rom = NULL;
  list_for_each_entry(current_entry, &sdb_rom->entry_list, list) {
     if (current_entry->data.record_type == sdb_type_bridge){
       struct sdb_bridge * tmp_bridge = (struct sdb_bridge *) &current_entry->data;
       printf("Bridge %*s%d@%08X: |%.19s|\n", level, "",current_entry->entry_id, tmp_bridge->sdb_component.addr_first, tmp_bridge->sdb_component.product.name);
       list_for_each_entry(current_rom, &sdb_rom->rom_list, list) {
         if (current_rom->entry_id == current_entry->entry_id) {
           wb_sdb_rom_dump(current_rom, level + 1);
           break;
         }
       }
     } else if (current_entry->data.record_type == sdb_type_device) {
       struct sdb_device * tmp_device = (struct sdb_device *) &current_entry->data;
       printf("Device %*s%d@%08X: |%.19s|\n", level, "",current_entry->entry_id, tmp_device->sdb_component.addr_first + sdb_rom->bus_address, tmp_device->sdb_component.product.name); 
      
     } else if (current_entry->data.record_type == sdb_type_interconnect) {
       struct sdb_interconnect * tmp_interconnect = (struct sdb_interconnect*) &current_entry->data;
       printf("Inter  %*s%d@%08X: |%.19s|\n", level, "", current_entry->entry_id, tmp_interconnect->sdb_component.addr_first + sdb_rom->bus_address, tmp_interconnect->sdb_component.product.name);
     }
  }
}


void wb_sdb_rom_discovery(struct pghal_bus * bus, struct wb_sdb_rom * sdb_rom, uint32_t bus_address)
{
  printf("wb_sdb_rom_discovery: 0x%08X\n", bus_address);
  struct sdb_empty tmp_entry;
  uint16_t records_count = 1;
  struct sdb_interconnect * tmp_inter = NULL;

  int i;
  struct wb_sdb_entry * current_entry = NULL;
  for(i=0; i<records_count; i++){
    wb_sdb_rom_read_entry(sdb_rom, i, &tmp_entry);
    switch (tmp_entry.record_type) {
       case sdb_type_bridge: ;
               struct sdb_bridge * tmp_bridge = decode_sdb_bridge(&tmp_entry);
               if (tmp_bridge == NULL) continue;
               
               break;
       case sdb_type_device: ;
               struct sdb_device * tmp_device = decode_sdb_device(&tmp_entry);
               if (tmp_device == NULL) continue;
               
               break;
       case sdb_type_interconnect: if (i != 0) continue;
               tmp_inter = decode_sdb_interconnect(&tmp_entry);
               if (tmp_inter == NULL) continue;
               records_count = tmp_inter->sdb_records;
               break;
       default: continue;
    }
        
    current_entry = (struct wb_sdb_entry *) pghal_alloc((sizeof(struct wb_sdb_entry)));
    current_entry->entry_id = i;
    current_entry->parent = sdb_rom;
    memcpy(&current_entry->data, &tmp_entry, sizeof(struct sdb_empty));
    list_add_tail(&current_entry->list, &sdb_rom->entry_list);
  }
  {
    //tmp_inter = list_entry(&sdb_rom->entry_list, (struct wb_sdb_rom), entry_list)
    current_entry = list_get_first(&sdb_rom->entry_list, list, typeof(*current_entry));
    tmp_inter  = (struct sdb_interconnect *) &current_entry->data; 
//    printf("Root address space: %08lX - %08lX\n", tmp_inter->sdb_component.addr_first + bus_address, tmp_inter->sdb_component.addr_last + bus_address);
  }
  i = 0;
  list_for_each_entry(current_entry, &sdb_rom->entry_list, list) {
     if (current_entry->data.record_type == sdb_type_bridge){
       struct wb_sdb_rom * tmp_down = wb_sdb_rom_create_child(sdb_rom, current_entry);
       if (tmp_down != NULL)
         list_add_tail(&tmp_down->list, &sdb_rom->rom_list);
     } else if (current_entry->data.record_type == sdb_type_device) {
       struct sdb_device * tmp_device = (struct sdb_device *) &current_entry->data;
     }
  }
  

  // we do not store empty enreis
 
  //printf("Addr: 0x%lX -> 0x%lX\n", first_entry->addr_first, first_entry->addr_last);
  
//  sdb_module_init(&gpio->sdb, bus, bus_address);

}

// creates rom from bridge entry
struct wb_sdb_rom * wb_sdb_rom_create_child(struct wb_sdb_rom * parent, struct wb_sdb_entry * sdb_entry) {
//struct wb_sdb_rom * tmp_down = wb_sdb_rom_create_direct(bus, tmp_bridge->sdb_component.addr_first, tmp_bridge->sdb_child);

  if (sdb_entry == NULL) return NULL;
  if (sdb_entry->data.record_type != sdb_type_bridge) return NULL;
  struct sdb_bridge * tmp_bridge = (struct sdb_bridge *) &sdb_entry->data; 

  struct wb_sdb_rom * sdb_rom = NULL;
  sdb_rom = (struct wb_sdb_rom *) pghal_alloc((sizeof(struct wb_sdb_rom)));

  sdb_rom->parent  = parent;
  sdb_rom->level   = parent->level+1;
  sdb_rom->sdb.bus = parent->sdb.bus;
  sdb_rom->sdb.address.sdb_address = tmp_bridge->sdb_child;
  sdb_rom->bus_address = tmp_bridge->sdb_component.addr_first ;
  
  INIT_LIST_HEAD(&sdb_rom->entry_list);
  INIT_LIST_HEAD(&sdb_rom->rom_list);

  sdb_rom->entry_id = sdb_entry->entry_id;
  printf("creating SDB level: %d, entry_id: %d\n", sdb_rom->level, sdb_rom->entry_id);
 
  // maybe sdb_rom->sdb.address.sdb_address + parent->bus_address;
  wb_sdb_rom_discovery(sdb_rom->sdb.bus, sdb_rom, sdb_rom->sdb.address.sdb_address);

  return sdb_rom;
}

struct wb_sdb_rom * wb_sdb_rom_create_direct(struct pghal_bus * bus,  uint32_t bus_address, uint32_t sdb_address)
{

  struct wb_sdb_rom * sdb_rom = NULL;
  sdb_rom = (struct wb_sdb_rom *) pghal_alloc((sizeof(struct wb_sdb_rom)));

  sdb_rom->sdb.bus = bus;
  sdb_rom->sdb.address.sdb_address =  sdb_address;
  INIT_LIST_HEAD(&sdb_rom->entry_list);
  INIT_LIST_HEAD(&sdb_rom->rom_list);
  sdb_rom->bus_address = bus_address;

  sdb_rom->parent = NULL;
  sdb_rom->level = 0;

  printf("creating top SDB\n");
  wb_sdb_rom_discovery(bus, sdb_rom, bus_address);

  return sdb_rom;
}


