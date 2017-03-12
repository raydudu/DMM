
void * operator new(size_t size) {
 return malloc(size);
}

void * operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void *pnt) {
    free(pnt);
}

void operator delete[](void *pnt) {
    free(pnt);
}

