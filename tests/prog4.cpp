#include <iostream>
#include <cstdlib>
#include <cstring>

#define POOL_SIZE 1024
using namespace std;

class Dummy {

};

static void alloc(Dummy **pool, int amount) {
    int i;
    cout << "Alloc: " << amount << endl;
    for(i = 0; i < POOL_SIZE; i++) {
        if (pool[i] == NULL) {
            pool[i] = new Dummy();
            amount--;
            if (amount == 0) {
                break;
            }
        }
    }
}

static void release(Dummy **pool, int amount) {
    int i;
    cout << "Release: " << amount << endl;
    for(i = 0; i < POOL_SIZE; i++) {
        if (pool[i] != NULL) {
            delete pool[i];
            pool[i] = NULL;
            amount--;
            if(amount == 0) {
                break;
            }
        }
    }
}

int main() {
    Dummy *pool[POOL_SIZE];
    int i;
    int count = 0;

    memset(pool, 0, sizeof pool);
    for(i = 0; i < 1114; i++) {
        switch(rand() % 2) {
            case 0:
                alloc(pool, random() % POOL_SIZE);
            break;
            case 1:
                release(pool, random() % POOL_SIZE);
            break;
            default:
                cout << "EER" << endl;
                abort();
        }
    }

    for(i = 0; i < POOL_SIZE; i++) {
        if (pool[i] != NULL){
            count++;
        }
    }

    cout << "Still allocated: " << count << endl;
    return 0;
}
