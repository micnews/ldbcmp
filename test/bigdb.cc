#include <iostream>
#include <thread>
#include <string>
#include <iomanip>

#include "leveldb/db.h"
#include "leveldb/write_batch.h"

using namespace std;

int keys_len = 1024*1024;

int batch_size = keys_len / 16;

string r = "04aad539c8dbaa3c2d91c32ae304c2036dd6c239907166cbae74bb4eaa2a73a0d94499552b0ad159d7304534dbeb78d2922097c7c6d64512e2ee8b17c4a41dd4c3a608ddbe3dde321d1dda349e7e8ee3ba3218d44e600a03a3c97dc6a7b9284ab2e013e5a385d3b3d15ca32a2a9e1536e81b355e17eca69de204947b546a91e3a701b96ac8e1b00021eb8e167059e9ac2accec57229e3a03ac7ecd26b9a869bda0db1a5e406172e5deda9ce87da23d6463675ebd6d64a513055a39b7e882a1ca9e9d16c0e7e51c963a29cec15585e14da0c96ca30a3e06cbca85b94ee0e84819a337ada0e18c9e0112b9348ee6060369e3142605d82c40502d116bcca1ea4d8aab9a00ee2b49171567a8ac54501274b83a2679c7072e607d0cdc42512b88b33db9d4ddbaa71966d449e8580e72aa554887117916674567656a183aba6eb38723cce7770573ed9d5ca98826b05dde950ad8c423c4187056075494572a7b79ba3ae5d2aaad81e6bb909678185713b04400405e69000aae2b53bb70968151cc5c85139589d4d103c17d10b82ce4821c52269bc259993e11d859b09c08cce0bb5e11363114797124ad2507586577b915be800d0ce5b1b3787c1a32288e204846bebe5d94398c525981767b48c101b99a9b1a74212c477904078531054758695d8eae8ad00a1d74a33b827c894e7183bbd6d7e00c0e4b809369a9ac3223875411bdd1bce3761cd810cd480b2a00d7167981d760c08bbdc846b388b5ae22ac07515c3604ce8c8a8c8";

void gen(leveldb::DB* db, leveldb::WriteOptions wo,  int id)
{
  leveldb::WriteBatch batch;
  const string sid = to_string(id) + "_";
  for (int i = 0; i < batch_size; i++)
  {
    batch.Put(sid + to_string(i), r);
  }
  db->Write(wo, &batch);
}

int main(int argc, char** argv)
{
  string path = "testdb";

  leveldb::DB* db;

  leveldb::Options options;
  options.create_if_missing = true;

  leveldb::Status status = leveldb::DB::Open(options, path, &db);

  if (!status.ok())
  {
    cerr << "db: " << status.ToString() << endl;
    return -1;
  }


  leveldb::WriteOptions writeOptions;
  thread tarr[16];

  for (int i=0; i < 16; i++) {
    tarr[i] = thread(gen, db, writeOptions, i);
  }

  for (int i=0; i < 16; i++) {
    tarr[i].join();
  }

  cout << keys_len << " keys generated with 1024 bytes of random data in each value using 16 threads";

  delete db;
}

// just out of curiosity, i timed this, node does really well.

// node
// 4.13 real         5.43 user         2.39 sys

// c++
// 3.08 real         3.22 user         2.98 sys

