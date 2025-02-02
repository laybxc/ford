// Author: Ming Zhang
// Copyright (c) 2022

#include "tatp_db.h"

#include "unistd.h"
#include "util/json_config.h"

/* Only initialize here. The worker threads will populate. */
void TATP::LoadTable(node_id_t node_id, node_id_t num_server) {
  // Initiate + Populate table for primary role
  if ((node_id_t)TATPTableType::kSubscriberTable % num_server == node_id) {
    printf("Primary: Initializing SUBSCRIBER table\n");
    // std::string config_filepath = "../../../workload/tatp/tatp_tables/subscriber.json";
    // auto json_config = JsonConfig::load_file(config_filepath);
    // auto table_config = json_config.get("table");
    // subscriber_table =
    //     new HashStore((table_id_t)TATPTableType::kSubscriberTable,
    //                   table_config.get("bkt_num").get_uint64(),
    //                   mem_store_alloc_param);
    PopulateSubscriberTable();
    primary_table_ptrs.push_back(subscriber_table);
  }
  if ((node_id_t)TATPTableType::kSecSubscriberTable % num_server == node_id) {
    printf("Primary: Initializing SECONDARY SUBSCRIBER table\n");
    // auto config_filepath = "../../../workload/tatp/tatp_tables/sec_subscriber.json";
    // auto json_config = JsonConfig::load_file(config_filepath);
    // auto table_config = json_config.get("table");
    // sec_subscriber_table =
    //     new HashStore((table_id_t)TATPTableType::kSecSubscriberTable,
    //                   table_config.get("bkt_num").get_uint64(),
    //                   mem_store_alloc_param);
    PopulateSecondarySubscriberTable();
    primary_table_ptrs.push_back(sec_subscriber_table);
  }
  if ((node_id_t)TATPTableType::kAccessInfoTable % num_server == node_id) {
    printf("Primary: Initializing ACCESS INFO table\n");
    // std::string config_filepath = "../../../workload/tatp/tatp_tables/access_info.json";
    // auto json_config = JsonConfig::load_file(config_filepath);
    // auto table_config = json_config.get("table");
    // access_info_table =
    //     new HashStore((table_id_t)TATPTableType::kAccessInfoTable,
    //                   table_config.get("bkt_num").get_uint64(),
    //                   mem_store_alloc_param);
    PopulateAccessInfoTable();
    primary_table_ptrs.push_back(access_info_table);
  }
  if ((node_id_t)TATPTableType::kSpecialFacilityTable % num_server == node_id) {
    printf("Primary: Initializing SPECIAL FACILITY table\n");
    // std::string config_filepath = "../../../workload/tatp/tatp_tables/special_facility.json";
    // auto json_config1 = JsonConfig::load_file(config_filepath);
    // auto table_config1 = json_config1.get("table");
    // special_facility_table = new HashStore((table_id_t)TATPTableType::kSpecialFacilityTable,
    //                                        table_config1.get("bkt_num").get_uint64(), mem_store_alloc_param);
    // printf("Primary: Initializing CALL FORWARDING table\n");
    // config_filepath = "../../../workload/tatp/tatp_tables/call_forwarding.json";
    // auto json_config2 = JsonConfig::load_file(config_filepath);
    // auto table_config2 = json_config2.get("table");
    // call_forwarding_table =
    //     new HashStore((table_id_t)TATPTableType::kCallForwardingTable,
    //                   table_config2.get("bkt_num").get_uint64(),
    //                   mem_store_alloc_param);
    PopulateSpecfacAndCallfwdTable();
    primary_table_ptrs.push_back(special_facility_table);
    primary_table_ptrs.push_back(call_forwarding_table);
  }

  // Initiate + Populate table for backup role
  if (BACKUP_DEGREE < num_server) {
    for (node_id_t i = 1; i <= BACKUP_DEGREE; i++) {
      if ((node_id_t)TATPTableType::kSubscriberTable % num_server == (node_id - i + num_server) % num_server) {
        // Meaning: I (current node_id) am the backup-SubscriberTable of my primary. My primary-SubscriberTable
        // resides on a node, whose id is TATPTableType::kSubscriberTable % num_server
        // A possible layout: | P (My primary) | B1 (I'm here) | B2 (Or I'm here) |
        printf("Backup: Initializing SUBSCRIBER table\n");
        // std::string config_filepath = "../../../workload/tatp/tatp_tables/subscriber.json";
        // auto json_config = JsonConfig::load_file(config_filepath);
        // auto table_config = json_config.get("table");
        // subscriber_table =
        //     new HashStore((table_id_t)TATPTableType::kSubscriberTable,
        //                   table_config.get("bkt_num").get_uint64(),
        //                   mem_store_alloc_param);
        PopulateSubscriberTable();
        backup_table_ptrs.push_back(subscriber_table);
      }

      if ((node_id_t)TATPTableType::kSecSubscriberTable % num_server == (node_id - i + num_server) % num_server) {
        printf("Backup: Initializing SECONDARY SUBSCRIBER table\n");
        // auto config_filepath = "../../../workload/tatp/tatp_tables/sec_subscriber.json";
        // auto json_config = JsonConfig::load_file(config_filepath);
        // auto table_config = json_config.get("table");
        // sec_subscriber_table =
        //     new HashStore((table_id_t)TATPTableType::kSecSubscriberTable,
        //                   table_config.get("bkt_num").get_uint64(),
        //                   mem_store_alloc_param);
        PopulateSecondarySubscriberTable();
        backup_table_ptrs.push_back(sec_subscriber_table);
      }

      if ((node_id_t)TATPTableType::kAccessInfoTable % num_server == (node_id - i + num_server) % num_server) {
        printf("Backup: Initializing ACCESS INFO table\n");
        // std::string config_filepath = "../../../workload/tatp/tatp_tables/access_info.json";
        // auto json_config = JsonConfig::load_file(config_filepath);
        // auto table_config = json_config.get("table");
        // access_info_table =
        //     new HashStore((table_id_t)TATPTableType::kAccessInfoTable,
        //                   table_config.get("bkt_num").get_uint64(),
        //                   mem_store_alloc_param);
        PopulateAccessInfoTable();
        backup_table_ptrs.push_back(access_info_table);
      }

      if ((node_id_t)TATPTableType::kSpecialFacilityTable % num_server == (node_id - i + num_server) % num_server) {
        printf("Backup: Initializing SPECIAL FACILITY table\n");
        // std::string
        //     config_filepath = "../../../workload/tatp/tatp_tables/special_facility.json";
        // auto json_config1 = JsonConfig::load_file(config_filepath);
        // auto table_config1 = json_config1.get("table");
        // special_facility_table = new HashStore((table_id_t)TATPTableType::kSpecialFacilityTable,
        //                                        table_config1.get("bkt_num").get_uint64(),
        //                                        mem_store_alloc_param);
        // printf("Backup: Initializing CALL FORWARDING table\n");
        // config_filepath = "../../../workload/tatp/tatp_tables/call_forwarding.json";
        // auto json_config2 = JsonConfig::load_file(config_filepath);
        // auto table_config2 = json_config2.get("table");
        // call_forwarding_table =
        //     new HashStore((table_id_t)TATPTableType::kCallForwardingTable,
        //                   table_config2.get("bkt_num").get_uint64(),
        //                   mem_store_alloc_param);
        PopulateSpecfacAndCallfwdTable();
        backup_table_ptrs.push_back(special_facility_table);
        backup_table_ptrs.push_back(call_forwarding_table);
      }
    }
  }
  fflush(stdout);
}

void TATP::PopulateSubscriberTable() {
  rm_manager->create_file(bench_name + "_subscriber", sizeof(tatp_sub_val_t));
  std::unique_ptr<RmFileHandle> table_file = rm_manager->open_file(bench_name + "_subscriber");
  std::ofstream indexfile;
  indexfile.open(bench_name + "_subscriber_index.txt");

  /* All threads must execute the loop below deterministically */
  uint64_t tmp_seed = 0xdeadbeef; /* Temporary seed for this function only */
  int total_records_inserted = 0, total_records_examined = 0;

  /* Populate the table */
  for (uint32_t s_id = 0; s_id < subscriber_size; s_id++) {
    tatp_sub_key_t key;
    key.s_id = s_id;

    /* Initialize the subscriber payload */
    tatp_sub_val_t sub_val;
    sub_val.sub_number = SimpleGetSubscribeNumFromSubscribeID(s_id);

    for (int i = 0; i < 5; i++) {
      sub_val.hex[i] = FastRand(&tmp_seed);
    }

    for (int i = 0; i < 10; i++) {
      sub_val.bytes[i] = FastRand(&tmp_seed);
    }

    sub_val.bits = FastRand(&tmp_seed);
    sub_val.msc_location = tatp_sub_msc_location_magic; /* Debug */
    sub_val.vlr_location = FastRand(&tmp_seed);

    total_records_inserted += LoadRecord(
        table_file.get(),
        key.item_key,
        (void*)&sub_val,
        sizeof(tatp_sub_val_t),
        (table_id_t)TATPTableType::kSubscriberTable,
        indexfile);
    total_records_examined++;
    // std::cerr << "total_records_examined: " << total_records_examined << std::endl;
  }
  
  indexfile.close();
}

void TATP::PopulateSecondarySubscriberTable() {
  rm_manager->create_file(bench_name + "_sec_subscriber", sizeof(tatp_sec_sub_val_t));
  std::unique_ptr<RmFileHandle> table_file = rm_manager->open_file(bench_name + "_sec_subscriber");
  std::ofstream indexfile;
  indexfile.open(bench_name + "_sec_subscriber_index.txt");

  int total_records_inserted = 0, total_records_examined = 0;

  /* Populate the tables */
  for (uint32_t s_id = 0; s_id < subscriber_size; s_id++) {
    tatp_sec_sub_key_t key;
    key.sub_number = SimpleGetSubscribeNumFromSubscribeID(s_id);

    /* Initialize the subscriber payload */
    tatp_sec_sub_val_t sec_sub_val;
    sec_sub_val.s_id = s_id;
    sec_sub_val.magic = tatp_sec_sub_magic;

    total_records_inserted += LoadRecord(
        table_file.get(),
        key.item_key,
        (void*)&sec_sub_val,
        sizeof(tatp_sec_sub_val_t),
        (table_id_t)TATPTableType::kSecSubscriberTable,
        indexfile);
    total_records_examined++;
  }
  indexfile.close();
}

void TATP::PopulateAccessInfoTable() {
  rm_manager->create_file(bench_name + "_access_info", sizeof(tatp_accinf_val_t));
  std::unique_ptr<RmFileHandle> table_file = rm_manager->open_file(bench_name + "_access_info");
  std::ofstream indexfile;
  indexfile.open(bench_name + "_access_info_index.txt");

  std::vector<uint8_t> ai_type_values = {1, 2, 3, 4};

  /* All threads must execute the loop below deterministically */
  uint64_t tmp_seed = 0xdeadbeef; /* Temporary seed for this function only */
  int total_records_inserted = 0, total_records_examined = 0;

  /* Populate the table */
  for (uint32_t s_id = 0; s_id < subscriber_size; s_id++) {
    std::vector<uint8_t> ai_type_vec = SelectUniqueItem(&tmp_seed, ai_type_values, 1, 4);
    for (uint8_t ai_type : ai_type_vec) {
      /* Insert access info record */
      tatp_accinf_key_t key;
      key.s_id = s_id;
      key.ai_type = ai_type;

      tatp_accinf_val_t accinf_val;
      accinf_val.data1 = tatp_accinf_data1_magic;

      /* Insert into table if I am replica number repl_i for key */
      total_records_inserted += LoadRecord(
        table_file.get(),
        key.item_key,
        (void*)&accinf_val,
        sizeof(tatp_accinf_val_t),
        (table_id_t)TATPTableType::kAccessInfoTable,
        indexfile);
      total_records_examined++;
    }
  }
  indexfile.close();
}

/*
 * Which rows are inserted into the CALL FORWARDING table depends on which
 * rows get inserted into the SPECIAL FACILITY, so process these two jointly.
 */
void TATP::PopulateSpecfacAndCallfwdTable() {
  rm_manager->create_file(bench_name + "_special_facility", sizeof(tatp_specfac_val_t));
  std::unique_ptr<RmFileHandle> special_facility_table = rm_manager->open_file(bench_name + "_special_facility");
  std::ofstream indexfile1;
  indexfile1.open(bench_name + "_special_facility_index.txt");

  rm_manager->create_file(bench_name + "_call_forwarding", sizeof(tatp_callfwd_val_t));
  std::unique_ptr<RmFileHandle> call_forwarding_table = rm_manager->open_file(bench_name + "_call_forwarding");
  std::ofstream indexfile2;
  indexfile2.open(bench_name + "_call_forwarding_index.txt");

  std::vector<uint8_t> sf_type_values = {1, 2, 3, 4};
  std::vector<uint8_t> start_time_values = {0, 8, 16};

  int total_records_inserted = 0, total_records_examined = 0;

  /* All threads must execute the loop below deterministically */
  uint64_t tmp_seed = 0xdeadbeef; /* Temporary seed for this function only */

  /* Populate the tables */
  for (uint32_t s_id = 0; s_id < subscriber_size; s_id++) {
    std::vector<uint8_t> sf_type_vec = SelectUniqueItem(
        &tmp_seed, sf_type_values, 1, 4);

    for (uint8_t sf_type : sf_type_vec) {
      /* Insert the special facility record */
      tatp_specfac_key_t key;
      key.s_id = s_id;
      key.sf_type = sf_type;

      tatp_specfac_val_t specfac_val;
      specfac_val.data_b[0] = tatp_specfac_data_b0_magic;
      specfac_val.is_active = (FastRand(&tmp_seed) % 100 < 85) ? 1 : 0;

      total_records_inserted += LoadRecord(
          special_facility_table.get(), key.item_key,
          (void*)&specfac_val, sizeof(tatp_specfac_val_t),
          (table_id_t)TATPTableType::kSpecialFacilityTable,
          indexfile1);

      total_records_examined++;

      /*
       * The TATP spec requires a different initial probability
       * distribution of Call Forwarding records (see README). Here, we
       * populate the table using the steady state distribution.
       */
      for (size_t start_time = 0; start_time <= 16; start_time += 8) {
        /*
         * At steady state, each @start_time for <s_id, sf_type> is
         * equally likely to be present or absent.
         */
        if (FastRand(&tmp_seed) % 2 == 0) {
          continue;
        }

        /* Insert the call forwarding record */
        tatp_callfwd_key_t key;
        key.s_id = s_id;
        key.sf_type = sf_type;
        key.start_time = start_time;

        tatp_callfwd_val_t callfwd_val;
        callfwd_val.numberx[0] = tatp_callfwd_numberx0_magic;
        /* At steady state, @end_time is unrelated to @start_time */
        callfwd_val.end_time = (FastRand(&tmp_seed) % 24) + 1;

        total_records_inserted += LoadRecord(
            call_forwarding_table.get(), key.item_key,
            (void*)&callfwd_val, sizeof(tatp_callfwd_val_t),
            (table_id_t)TATPTableType::kCallForwardingTable,
            indexfile2);
        
        total_records_examined++;

      } /* End loop start_time */
    }   /* End loop sf_type */
  }     /* End loop s_id */
  indexfile1.close();
  indexfile2.close();
}

int TATP::LoadRecord(RmFileHandle* file_handle,
                      itemkey_t item_key,
                      void* val_ptr,
                      size_t val_size,
                      table_id_t table_id,
                      std::ofstream& indexfile
                      ) {
  assert(val_size <= MAX_ITEM_SIZE);
  /* Insert into Disk */
  DataItem item_to_be_inserted(table_id, val_size, item_key, (uint8_t*)val_ptr);
  char* item_char = (char*)malloc(item_to_be_inserted.GetSerializeSize());
  item_to_be_inserted.Serialize(item_char);
  Rid rid = file_handle->insert_record(item_key, item_char, nullptr);
  // record index
  indexfile << item_key << " " << rid.page_no_ << " " << rid.slot_no_ << std::endl;
  free(item_char);
  return 1;
}

/*
 * Select between N and M unique items from the values vector. The number
 * of values to be selected, and the actual values are chosen at random.
 */
std::vector<uint8_t> TATP::SelectUniqueItem(uint64_t* tmp_seed, std::vector<uint8_t> values, unsigned N, unsigned M) {
  assert(M >= N);
  assert(M >= values.size());

  std::vector<uint8_t> ret;

  int used[32];
  memset(used, 0, 32 * sizeof(int));

  int to_select = (FastRand(tmp_seed) % (M - N + 1)) + N;
  for (int i = 0; i < to_select; i++) {
    int index = FastRand(tmp_seed) % values.size();
    uint8_t value = values[index];
    assert(value < 32);

    if (used[value] == 1) {
      i--;
      continue;
    }

    used[value] = 1;
    ret.push_back(value);
  }
  return ret;
}
