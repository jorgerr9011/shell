//
// Created by jaime on 7/1/21.
//
#ifndef UNTITLED_LISTA2_H
#define UNTITLED_LISTA2_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

#define COMMAND	256

typedef struct {
	pid_t pid;
    int priority;
    char command[COMMAND];
    char start[128];
    char state[20]; //run, stop, term, sig
    int value; //Valor devuelto en finalización correcta o nº de señal
}Process;

typedef struct node{
    Process *process;
    struct node *next;
}Node;


void CreateListE(Node *lista);

void InsertElementE(Node *lista, Process *p);

void MostrarProceso(Process p);

void MostrarLista(Node lista);

void BorrarState(Node *lista, char *state);

int BorrarPID(Node *lista, int pid);

#endif //UNTITLED_LISTA2_H