//Lista con Array de Punteros a Elementos (3)

#include "lista.h"

List CreateList(List *list){
    list->size = 0;
    return *list;
}

List InsertElement(List *list, void *element){
    list->elements[list->size] = element;
    list->size ++;
    return *list;
}

List ClearList(List *list, void (*liberar)(void*)){
    list->size = 0;
    for(int i = 0; i < list->size; i++) liberar(list->elements[list->size]);
    return *list;
}
pos first(List *list){
    return 0;
}
pos next(List *list, pos actual){
    return actual + 1;
}
bool end(List *list, pos actual){
    if(list->size == actual) return true;
    else return false;
}
void* element(List *list, pos actual){
    return list->elements[actual];
}
int size(List *list){
    return list->size;
}
List deleteElementFree(List *list, pos actual){
    free(list->elements[actual]);
    for(pos n = actual; !end(list, n); n = next(list, n))
        list->elements[n] = list->elements[n + 1];
    list->elements[list->size - 1] = NULL;
    list->size--;
    return *list;
}
List deleteElement(List *list, pos actual){
    list->elements[actual] = NULL;
    for(pos n = actual; !end(list, n); n = next(list, n))
        list->elements[n] = list->elements[n + 1];
    list->elements[list->size - 1] = NULL;
    list->size--;
    return *list;
}