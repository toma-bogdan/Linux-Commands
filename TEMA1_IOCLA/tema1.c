#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

/*Functie care initializeaza un Director*/
Dir* createDirector(char* name){
	Dir* director = calloc(1,sizeof(Dir));
	director->name = calloc(strlen(name) + 10,sizeof(char));
	strcpy(director->name,name);
	director->parent = NULL;
	director->next = NULL;
	director->head_children_dirs = NULL;
	director->head_children_files = NULL;
	
	return director;
}

/*Functie care initializeaza un fisier*/
File* createFile(char* name){
	File* file = calloc(1,sizeof(File));
	file->name = calloc(strlen(name) + 10,sizeof(char));//schimba
	strcpy(file->name,name);
	file->parent = NULL;
	file->next = NULL;
	return file;
}

/*Functie care sterge un fisier*/
void delete_file(File* file){
	file->next = NULL;
	file->parent = NULL;
	free(file->name);
	free(file);
}

/*Functie care sterge un director si ierarhia lui de fisiere si directoare*/
void delete_director(Dir* director){

	while (director != NULL)
	{
		Dir *aux = director;
		if(director->head_children_dirs != NULL){
			delete_director(director->head_children_dirs);
		}
		while (director->head_children_files != NULL)
		{
			File *aux2 = director->head_children_files;
			director->head_children_files = director->head_children_files->next;
			delete_file(aux2);
		}
		director = director->next;
		aux->head_children_dirs = NULL;
		aux->head_children_files = NULL;
		aux->next = NULL;
		aux->parent = NULL;
		free(aux->name);
		free(aux);
	}
	
}

void touch (Dir* parent, char* name) {

	if(parent->head_children_files == NULL){
		//Cazul in care lista de fisiere este nula:
		File * aux = createFile(name);
		aux->parent = parent;
		parent->head_children_files = aux;
		return;
	}
	
	File *p = parent->head_children_files;
	File *last;
	for(; p != NULL; p = p->next){
		if(strcmp(p->name,name) == 0){
			printf("File already exists\n");
			return;
		}
		if(p->next == NULL)
			last = p;
	}
	File * aux = createFile(name);
	aux->parent = parent;
	last->next = aux;
}

void mkdir (Dir* parent, char* name) {

	if(parent->head_children_dirs == NULL){
		//Cazul in care lista de directoare este nula:
		Dir * aux = createDirector(name);
		aux->parent = parent;
		parent->head_children_dirs = aux;
		return;
	}
	Dir *p = parent->head_children_dirs;
	Dir *last;
	for(; p != NULL; p = p->next){
		if(strcmp(p->name,name) == 0){
			printf("Directory already exists\n");
			return;
		}
		if(p->next == NULL)
			last = p;
	}
	Dir * aux = createDirector(name);
	aux->parent = parent;
	last->next = aux;
}

void ls (Dir* parent) {
	Dir * dir = parent->head_children_dirs; 
	
	for (; dir != NULL; dir = dir->next){
		printf("%s\n",dir->name);
	}
	File *file = parent->head_children_files;

	for(; file != NULL; file = file->next){
		printf("%s\n",file->name);
	}
}

void rm (Dir* parent, char* name) {

	File *p = parent->head_children_files, *ant = NULL;
	for(; p != NULL;ant = p, p = p->next){
		if(strcmp(p->name,name) == 0){
			File *aux = p;
			if(ant == NULL){
				//Cazul in care stergem primul element din lista:
				parent->head_children_files = p->next;
			}else{
				//Cazul in care stergem de la mijlocul, respectiv finalul listei:
				ant->next = p->next;
			}
			delete_file(aux);
			return;
		}
	}
	printf("Could not find the file\n");
}

void rmdir (Dir* parent, char* name){

	Dir *p = parent->head_children_dirs, *ant = NULL;
	for(; p != NULL;ant = p, p = p->next){
		if(strcmp(p->name,name) == 0){
			if (ant == NULL){
				//Cazul in care stergem primul element din lista:
				parent->head_children_dirs = p->next;
			}else{
				//Cazul in care stergem de la mijlocul, respectiv finalul listei:
				ant->next = p->next;
			}
			//Stergem ierarhia de directoare si fisiere a directorului:
			delete_director(p->head_children_dirs);
			
			//Stergem fisierele directorului:
			while(p->head_children_files != NULL){
				File * aux = p->head_children_files;
				p->head_children_files = p->head_children_files->next;
				delete_file(aux);
			}
			//Eliberam memoria:
			p->head_children_dirs = NULL;
			p->head_children_files = NULL;
			p->next = NULL;
			p->parent = NULL;
			free(p->name);
			free(p);
			return;
		}
	}
	printf("Could not find the dir\n");
}

void cd(Dir** target, char *name) {
	if(strcmp(name,"..") == 0){
		if((*target)->parent != NULL){
			(*target) = (*target)->parent;
		}
		return;
	}
	for(Dir* p = (*target)->head_children_dirs; p != NULL; p = p->next){
		if(strcmp(p->name,name) == 0){
			(*target) = p;
			return;
		}
	}
	printf("No directories found!\n");
}

char *pwd (Dir* target) {
	char *path = calloc(MAX_INPUT_LINE_SIZE,sizeof(char));
	char *p = calloc(MAX_INPUT_LINE_SIZE,sizeof(char));// va retine mereu "/directorul_curent"
	char *aux; //retine o copie a path-ului
	strcpy(path,"/");
	strcat(path,target->name);

	while (target->parent != NULL)
	{
		target = target->parent;

		strcpy(p,"/");
		strcat(p,target->name);

		aux = strdup(path);

		strcpy(path,p);
		strcat(path,aux);

		free(aux);
	}
	free(p);
	
	return path;
}

void stop (Dir** target) {

	while((*target)->parent != NULL){
		*target = (*target)->parent;
	}
	delete_director((*target)->head_children_dirs);

	while((*target)->head_children_files != NULL){
		File *aux = (*target)->head_children_files;
		(*target)->head_children_files = (*target)->head_children_files->next;
		delete_file(aux);
	}
	free((*target)->name);
	free(*target);
}

void tree (Dir* target, int level) {

	while (target != NULL)
	{
		if(level != 0){
			for(int i = 1; i < level; i++){
				printf("    ");
			}
			printf("%s\n",target->name);
		}
		if(target->head_children_dirs != NULL){
			tree(target->head_children_dirs,level + 1);
		}

		for(File *p = target->head_children_files; p != NULL; p = p->next){
			for(int i = 1; i <= level; i++)
				printf("    ");
			printf("%s\n",p->name);
		}
		target = target->next;
	}

}

void mv(Dir* parent, char *oldname, char *newname) {}

int main () {

	Dir* home = createDirector("home");
	char* line = calloc(MAX_INPUT_LINE_SIZE,sizeof(char*));

	do
	{
		fgets(line, MAX_INPUT_LINE_SIZE, stdin);
		line[strlen(line) - 1] = '\0';

		char *command = strtok(line," \n");

		if(strcmp(command,"touch") == 0){
			char *name = strtok(NULL," \n");
			touch(home,name);
		}
		if(strcmp(command,"mkdir") == 0){
			char *name = strtok(NULL," \n");
			mkdir(home,name);
		}
		if(strcmp(command,"ls") == 0){
			ls(home);
		}
		if(strcmp(command,"rm") == 0){
			char *name = strtok(NULL," \n");
			rm(home,name);
		}
		if(strcmp(command,"rmdir") == 0){
			char *name = strtok(NULL," \n");
			rmdir(home,name);
		}
		if(strcmp(command,"cd") == 0){
			char *name = strtok(NULL," \n");
			cd(&home,name);
		}
		if(strcmp(command,"tree") == 0){
			tree(home,0);
		}
		if(strcmp(command,"pwd") == 0){
			char *path = pwd(home);
			printf("%s\n",path);
			free(path);
		}
		if(strcmp(command,"stop") == 0){
			stop(&home);
			free(line);
			break;
		}
	} while (1);


	return 0;
}
