//
// Created by jaime on 29/10/20.
//

#ifndef UNTITLED_LISTA_H
#define UNTITLED_LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define ELEMENTS 4096

typedef int pos;
typedef struct {
    void *elements[ELEMENTS];
    int size;
} List;

List CreateList(List *list);
List InsertElement(List *list, void *element);
List ClearList(List *list, void (*liberar)(void*));
pos first(List *list);
pos next(List *list, pos actual);
bool end(List *list, pos actual);
void* element(List *list, pos actual);
int size(List *list);
List deleteElementFree(List *list, pos actual);
List deleteElement(List *list, pos actual);



#endif //UNTITLED_LISTA_H