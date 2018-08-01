#ifndef __WB_SDB_ROM_H__
#define __WB_SDB_ROM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SDB_ID_DEVICE 0x01
#define SDB_ID_DEVICE 0x01
#define SDB_ID_DEVICE 0x01


#define SDB_SEARCH_BY_TYPE  0x01
#define SDB_SEARCH_BY_NAME  0x02
#define SDB_SEARCH_BY_ID    0x01

#define SDB_SEARCH_EXACT    0xFF

// 64 bytes = 512 bits
#define SDB_RECORD_LENGTH 64


struct wb_sdb_rom {
  struct sdb_module sdb;

  struct wb_sdb_rom * parent; // parent sdb_rom
  struct pghal_list list; // list of all sdb_roms at the same level
  uint32_t entry_id;      // id that matches bridge entry id
  
  uint32_t level;  
  uint32_t bus_address;
  // SDB entry
  //  struct wb_sdb_entry * entries;

  struct pghal_list  entry_list;  // list of sub modules
  struct pghal_list  rom_list; // list of sub bridges
};



struct wb_sdb_rom * wb_sdb_rom_create_direct(struct pghal_bus * bus, uint32_t bus_address, uint32_t sdb_address);

struct wb_sdb_rom * wb_sdb_discovered(struct wb_sdb_rom * sdb_rom);

struct wb_sdb_rom * wb_sdb_get_sdb_rom_by_id(struct wb_sdb_rom * sdb_rom, uint16_t id);
struct wb_sdb_rom * wb_sdb_get_sdb_rom_by_ids(struct wb_sdb_rom * sdb_rom, char * id_string);

enum sdb_record_type wb_sdb_get_type_by_id(struct wb_sdb_rom * sdb_rom, uint16_t id); // return id or empty
enum sdb_record_type wb_sdb_get_type_by_ids(struct wb_sdb_rom * sdb_rom, char * id_string); // return id or empty

int wb_sdb_get_name_by_id(struct wb_sdb_rom * sdb_rom, uint16_t id, char * name);
int wb_sdb_get_addr_by_id(struct wb_sdb_rom * sdb_rom, uint16_t, uint32_t * sdb_address);
int wb_sdb_get_name_by_ids(struct wb_sdb_rom * sdb_rom, char * id_string, char * name);
int wb_sdb_get_addr_by_ids(struct wb_sdb_rom * sdb_rom, char * id_string, uint32_t * sdb_address);
void wb_sdb_rom_dump(struct wb_sdb_rom * sdb_rom, int level);


#ifdef __cplusplus
}
#endif

#endif
