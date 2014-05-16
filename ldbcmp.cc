#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <unordered_map>
#include <getopt.h>

#include "leveldb/db.h"
#include "deps/cityhash/src/city.h"
#include "ldbcmp.h"

static int errors = 0;

void cmp_hash64(uint64 expected, uint64 actual);
void iterate_set(leveldb::DB* db, unordered_map<string, uint64> &hashes);

int main(int argc, char** argv)
{

  unordered_map<string, uint64> valmap;

  string path_a = argv[1];
  string path_b = argv[2];

  leveldb::DB* db_a;
  leveldb::DB* db_b;

  leveldb::Options options;
  options.create_if_missing = false;

  leveldb::Status status_a = leveldb::DB::Open(options, path_a, &db_a);
  leveldb::Status status_b = leveldb::DB::Open(options, path_b, &db_b);

  if (status_a.ok() == false || status_b.ok() == false)
  {
    cerr << "db_a: " << status_a.ToString() << endl;
    cerr << "db_b: " << status_b.ToString() << endl;
    return -1;
  }

  thread th1(&iterate_set, db_a, ref(valmap));
  thread th2(&iterate_set, db_b, ref(valmap));

  th1.join();
  th2.join();

  delete db_a;
  delete db_b;
  return 0;
}

//cmp_hash64(u1, u2);

void cmp_hash64(uint64 expected, uint64 actual)
{
  if (expected != actual) 
  {
    cerr << "ERROR: expected 0x" << hex << expected << ", but got 0x" << actual << "\n";
    ++errors;
  }
}

void iterate_set(leveldb::DB* db, unordered_map<string, uint64> &hashes)
{
  leveldb::Iterator* itr = db->NewIterator(leveldb::ReadOptions());
  leveldb::Slice start = "";

  for (itr->Seek(start); itr->Valid(); itr->Next())
  {
    leveldb::Slice key = itr->key();
    leveldb::Slice value = itr->value();

    string sKey = key.ToString();
    string sValue = key.ToString();
    string data = sKey + sValue;

    const char *buf = data.c_str();
    size_t len = data.length();

    const uint64 h = CityHash64(buf, len);

    // cout << sKey << " -> " << hex << h;

    hashes.emplace(sKey, h);

    auto it = hashes.find(sKey);

    cout << it->first << it->second;
    //if (it != hashes.end() && it->second == h) {
    //  hashes.erase(it->begin());
    //}
    //else {
    //  hashes.emplate(sKey, h);
    //}
  }
  delete itr;
}

