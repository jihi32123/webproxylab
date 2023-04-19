#include "cache.h"

/* 새로운 캐시를 생성하는 함수*/
cache *new_cache(){ 
    cache * c = malloc(sizeof(cache));
    c->root = NULL;
    c->tail = NULL;
    c->size = 0;
    return c;
}
/* 새로운 캐시 노드를 생성하는 함수 */
// 노드를 동적으로 할당해주고 포인터 리턴 
//삭제할 때 각 포인터들도 지워야함 메모리 관리에 주의하기!
cache_node *new_cache_node(char *key , char* value){
    cache_node *node = malloc(sizeof(cache_node)); 

    node->key = malloc(strlen(key) + 1);
    strcpy(node->key, key);
    node->value = malloc(strlen(value) + 1); 
    strcpy(node->value, value);
    
    return node;
}

/* 동적할당된 노드를 반환하는 함수 */
void free_node(cache_node *free_node){
    free(free_node->key);
    free(free_node->value);
    free(free_node);
}

/* 노드를 삭제하는 함수 */
void delete_node(cache * target_cache, cache_node *delete_node){

    // 1. 삭제 노드 prev랑 next를 연결한다. 
    // 1.1 prev 노드의 next를 삭제 노드의 next로 변경
    if(delete_node->prev != NULL)                    
        delete_node->prev->next = delete_node->next;
    else
        target_cache->root = delete_node->next;
    // 1.2 next 노드의 prev를 삭제 노드의 prev로 변경
    if(delete_node->next != NULL)
        delete_node->next->prev = delete_node->prev; 
    else 
        target_cache->tail = delete_node->prev;

    // 2. 해당 노드를 free 해준다.(내부 필드를 포인터로 생성했기 때문에 프리해주는 함수를 따로 만들었다.) 
    free_node(delete_node);
}

//  3. 데이터 찾기 (find_value) : 시간이 남으면 해시 테이블로 구현하자
//         1. key를 입력하면 데이터 찾기 
//         2. 데이터를 찾으면 1 반환 (buf 값을 인자로 받고 넣어주면 데이터는 거기에 값 채우기)
//             데이터를 찾으면 지금 찾은 데이터를 삭제하고, 링크드 리스트 가장 앞에 넣어주자(LRU를 위해)
//         3. 데이터를 못 찾으면 0 반환 
int find_cache(cache *target_cache, char *key, char *buf)
{
    printf("find_cache / key = %s!!!!\n", key);
    cache_node *start = target_cache->root;// 시작 노드 설정
    while(start != NULL){
        if(strcmp(start->key,key) == 0){ // 같은 키 값을 찾으면?
            strcpy(buf,start->value);
            printf("key = %s\n", key);
            // 이전에 있던 캐시를 삭제하고, 가장 앞에 넣는다. 
            insert_cache(target_cache, start->key, start->value);
            delete_node(target_cache, start);
            return 1;
        }
        start = start->next;
    }
    return 0;
}
int print_cache(cache *target_cache){
    printf("start print cache!\n");
    cache_node *start = target_cache->root;// 시작 노드 설정
    printf("cache size = %d\n", target_cache->size);
    printf("cache root = %s\n", target_cache->root);
    printf("cache root = %s\n", target_cache->tail);
    printf("==============\n");
    while(start != NULL){
        printf("node key = %s\n", start->key);
        printf("node vaue = %s\n", start->value);
        printf("==============\n");
        start = start->next;
    }
}
// 2. 데이터 저장
//     // 조건
//     1. 최대 오브젝트 사이즈 이하만 저장
//     2. 저장은 무조건 가장 앞에
//     3. 만약 넣으려는데 버퍼 사이즈 초과했다면?
//     4. 가장 오래 된 것을 찾아서 삭제하자
int insert_cache(cache *target_cache, char* key, char *value){
    printf("insert cache!\n");

    // 만약 최대 값을 넘어가면 읽은지 가장 오래된 노드를 제거한다.
    while (target_cache->size + strlen(value) > MAX_CACHE_SIZE){
        delete_node(target_cache, target_cache->tail);
    }
    target_cache->size += strlen(value); // 사이즈를 키운다.
    
    // 1. 새로운 노드 생성하기
    cache_node *new_node= new_cache_node(key, value);
    // 2. 노드를 캐시에 추가한다. 
    // 2.1. 새로운 노드의 next 포인터를 최선 노드로 한다.
    new_node->next = target_cache->root;
    // 2.2. 최선 노드에 prev 포인터가 새로운 노드를 가리키도록 한다. 
    // 만약 root가 널이면?
    if (target_cache->root != NULL)
        target_cache->root->prev = new_node;
    else 
        target_cache->tail = new_node;
    // 2.3. 캐시의 루트 포인터가 새로운 노드를 가리키도록 한다. 
    target_cache->root = new_node;
    
}
