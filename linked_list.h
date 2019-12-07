#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Node for linked list
typedef struct node{
	char* name;
	int fd;
	char* message;
	struct node* next;
}node_t;

node_t* head = NULL; //head of linked list

//Function to add to linked list
void* push(int fd, char* c_name)
{
	//Check for error cases
	if (head != NULL){
		node_t* temp = head;
		if (strncmp(temp->name, c_name, strlen(temp->name))==0){
			printf("Error: This client identifier is already in use! \n");
			return NULL;
		}
		while ((temp->next) != NULL){
			if (strncmp(temp->name, c_name, strlen(temp->name))==0){
				printf("Error: This client identifier is already in use! \n");
				return NULL;
			}
			temp = temp->next;
		}
		//Add node to linked list, update attributes for node
		printf("Adding to list...\n");
		node_t* n = (node_t*)malloc(sizeof(node_t));
		n->name = c_name;
		n->fd = fd;
		temp-> next = n;
		n-> next = NULL;
		return n;
	}
	else
	{
		//Add node to linked list, update attributes for node
		printf("Adding to list...\n");
		node_t* n = (node_t*)malloc(sizeof(node_t));
		n->name = c_name;
		n->fd = fd;
		n->next = NULL;
		head = n;	
		return n;
	}
}

//Function to remove node from linked list
void delete_node(int fd)
{
	//If list is empty
	if (head == NULL){
		printf("Error: There are no clients connected to the server.\n");
	}
	else{
		//Delete node if found
		node_t* current = head;
		node_t* previous = NULL;

		while ((current->fd) != fd){
			previous = current;
			current = current->next;
		}	
		if (current == head){
			current = head -> next;
			head = current;
			free(current);
		}
		else{
			previous->next = current->next;
			free(current);
		}
	}
	

}

//Show the entire linked list
void display()
{
	node_t* temp = head;
	while (temp != NULL){
		printf("%s\n", temp->name);
		temp = temp->next;
	}
	temp = NULL;
}