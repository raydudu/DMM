#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../external/htable.h"

static htable_entry_t *new_entry(char *key, char *val){
    htable_entry_t *e;
    e = malloc(sizeof(htable_entry_t));
    e->key = key;
    e->key_len = strlen(key);
    e->data = val;
}

static htable_entry_t *print_valget(htable_t *ht, char *key) {
    htable_entry_t *e;

    e = htable_get(ht, key, strlen(key));
    if (e == NULL){
        printf("KeyGET: %s == NULL\n", key);
    } else {
        printf("KeyGET: %s == %s\n", key, (char *)e->data);
    }
    return e;
}

static void print_valpop(htable_t *ht, char *key) {
    htable_entry_t *e;

    e = htable_pop(ht, key, strlen(key));
    if (e == NULL){
        printf("KeyPOP: %s == NULL\n", key);
    } else {
        printf("KeyPOP: %s == %s\n", key, (char *)e->data);
        free(e);
    }

}

int main(int argc, char **argv) {

    htable_t *ht = htable_new(7);

    htable_dump(ht);

    htable_push(ht, new_entry("112", "inky"));
    htable_push(ht, new_entry("key2", "pinky"));
    htable_push(ht, new_entry("key3", "blinky"));
    htable_push(ht, new_entry("key4", "floyd"));

    htable_dump(ht);

    print_valget(ht, "112");
    print_valget(ht, "key2");
    print_valpop(ht, "key3");
    print_valpop(ht, "key4");
    print_valget(ht, "EINVAL");
    print_valpop(ht, "EINVAL");

    htable_dump(ht);

    htable_delete(ht, free);

    return 0;
}
