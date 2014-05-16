# SYNOPSIS
Compare two leveldb instances.

# DESCRIPTION
The levelDB iterator reads keys out of the database but never loads the 
entire database into memory. So essentially we have two streams that can
be read in two threads concurrently.

As the streams are read, each value is hashed and the key is added to an
unordered map with the hash as the value. Each thread will become mutually
exclusive when it needs to write to the value map.

Both threads are a stream of ordered keys. Each thread will repeat the
following steps in parallel...

Check if the key has been added
  A. The key has *not* been added, hash the value and add it to the value map.
  B. The key has been added, remove it from the map.

We want each database's iterator to produce duplicate keys and values so that 
they can be deduplicated. After the streams have ended we can determine
if the deduplication process has been sucessful if the length of the map is `0`.

# USAGE
Specify two database paths.

```
./ldbcmp ./fixtures/same_a ./fixtures/same_b
```

