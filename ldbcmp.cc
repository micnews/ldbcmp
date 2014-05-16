#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "leveldb/db.h"
#include "deps/cityhash/src/city.h"
#include "ldbcmp.h"

static int errors = 0;
mutex mtx;

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

  for (auto & item : valmap) {
    cout << item.first << hex << item.second << endl;
  }

  cout << valmap.size();

  delete db_a;
  delete db_b;
  return 0;
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

    auto *buf = data.c_str();
    auto len = data.length();

    auto h = CityHash64(buf, len);

    mtx.lock();

    auto it = hashes.find(sKey);

    if (it != hashes.end()) {
      // cout << "-" << sKey << " => " << hex << it->second << endl;
      hashes.erase(it);
    }
    else {
      // cout << "+" << sKey << " => " << hex << h << endl;
      pair<string, uint64> item(sKey, h);
      hashes.insert(item);
    }

    mtx.unlock();
  }
  delete itr;
}

