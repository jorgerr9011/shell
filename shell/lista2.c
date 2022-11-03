//Lista enlazada con nodo cabecera (1)

#include "lista2.h"

#define COMMAND	256

void CreateListE(Node *list){
	list = (Node*)malloc(sizeof(Node));
	(list)->next = NULL;
}

void InsertElementE(Node *list, Process *process){
	Node *node =(Node *) malloc(sizeof(Node));
    node->process = process;
    node->next = list->next;
	list->next = node;
}

void MostrarProceso(Process p){
    if (strcmp(p.state,"RUNNING") == 0){
        printf("pid: %d priority: %d command: %s time: %s process state: %s\n", p.pid, p.priority, p.command, p.start, p.state);
    }
    else{
        printf("pid: %d priority: %d command: %s time: %s process state: %s %d\n", p.pid, p.priority, p.command, p.start, p.state, p.value);
    }
}

void MostrarLista(Node lista){
	Node aux = lista;
	while(aux.next != NULL){
		aux = *aux.next;
		MostrarProceso(*aux.process);
	}
}

void BorrarState(Node *lista, char *state){
		
	Node *aux;;
	Node *borrar;
		
	aux = lista;
	while(aux->next != NULL){
		if(strcmp(aux->next->process->state,state) == 0){
			borrar = aux->next;
			if(aux->next->next != NULL){
				aux->next = aux->next->next;
			}else{
				aux->next = NULL;
			}
			free(borrar->process);
			free(borrar);
		}else{
			aux= aux->next;
		}
	}
}

int BorrarPID(Node *lista, int pid){
	
	Node *aux;
	Node *borrar;
	
	aux = lista;
	while(aux->next != NULL){
		if(aux->next->process->pid == pid){
			borrar = aux->next;
			if(aux->next->next != NULL){
				aux->next = aux->next->next;
			}else{
				aux->next = NULL;
			}
			free(borrar->process);
			free(borrar);					
			return 0;
		}else{
			aux= aux->next;
		}
	}
	return -1;
}