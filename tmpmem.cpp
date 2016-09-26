#include <stdio.h>
#include <stdlib.h>
#include <libmemcached/memcached.h>
#include <iostream>
#include <string>
using namespace std;


int main(int argc, char *argv[]) {
  // codes should be added for init
  memcached_server_st *servers = memcached_servers_parse("127.0.0.1:6339"); // server ip & port
  memcached_st *memc = memcached_create(NULL); // 看成一个放value对的库就行了
  memcached_server_push(memc, servers); // 放到server上去
  memcached_server_list_free(servers);
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 0);


  string strKey = "1";
  string strValue = "ahahahahahaha, you loser"; // No space allowd!!!!!!!!!!!!!!!!!!!!!
  // yi, with spaces there is no error........
  memcached_return rc = memcached_set(memc, strKey.c_str(), strlen(strKey.c_str()),
                                      strValue.data(), strValue.size(), 0, 0);


  if (rc == MEMCACHED_SUCCESS) { // set key success
    printf("memcached_set ok! \n");
    size_t ValueLength = 0;
    uint32_t flags;
    char *pValue = memcached_get(memc, strKey.c_str(), strlen(strKey.c_str()), &ValueLength, &flags, &rc);
    if (rc == MEMCACHED_SUCCESS) {
      printf("memcached_get key=%s value=%s \n", strKey.c_str(), pValue);
      free(pValue); // ???
    }
  }
  memcached_free(memc);
  return 0;
}
