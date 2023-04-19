# include "cache.h"

int main(){
    cache *c = new_cache();
    printf(">> insert_cache\n");
    char key1[] = "key1";
    char value1[] = "value1";
    insert_cache(c, key1, value1);

    char key2[] = "key2";
    char value2[] = "value3";
    insert_cache(c, key2, value2);

    char key3[] = "key3";
    char value3[] = "value3";
    insert_cache(c, key3, value3);

    printf(">> print_csache\n");
    print_cache(c);
    char buf[100000];
    if(find_cache(c, &key3, &buf)){
        printf("%s \n",buf);
    }
    return 0;
}