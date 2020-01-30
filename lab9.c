#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define IS_DEPT(x) ((x)>=1 && (x)<=4)

union info{
	float fever;
	char pain[10];
	int days;
};

#define NODE struct node
struct node{
	int department;
	char name[10];
	union info details;
	NODE *next;
};

struct head_tail{
	NODE *head;
	NODE *tail;
};
struct head_tail wait_list[4];

pthread_mutex_t mutex;

//Check if a name is already in the list.
int check_duplicate (char name[]){ 
	int x, i;
	NODE *p;
	for(i=0;i<4;i++){
		p=wait_list[i].head;
		while(p!=NULL){
			x=strcmp(p->name, name);
			if(x==0){
				return 0;
			}
			p=p->next;
		}
	}
	return 1;
}

//Get department number from the user.
int get_department(){
	int department;
	while(1){
		scanf("%d", &department);
		if(IS_DEPT(department))
			break;
		else{
			printf("Sorry that is not a valid department.\n");
			printf("Please try again(1,2,3,4): ");
		}
	}
	return department;
}

union info get_details(int department){
	union info details;
	switch(department){
		case 1: 
			printf("What is your temperature?: ");
			scanf("%f", &details.fever);
			break;
		case 2: 
			printf("Where is your pain?: ");
			scanf("%s", details.pain);
			break;
		case 3:
		case 4:
			printf("How many days have you felt sick?: ");
			scanf("%d", &details.days);
			break;  
	}
	return details;
}

//Empty List
void clearlist(NODE *p){
	if(p!=NULL){
		clearlist(p->next);
		free(p);
	}
	return;
}

//Insert a name to the list.
void insert(char name[10], int department, union info details){
	NODE *p;
	p=(NODE *)malloc(sizeof(NODE));
	if(p==NULL){
		printf("System Error. Memory Full.");
		return; 
	}
	strcpy(p->name, name);
	p->department=department;
	switch(department){
		case 1:
			p->details.fever=details.fever;
			break;
		case 2:
			strcpy(p->details.pain,details.pain);
			break;
		case 3:
		case 4:
			p->details.days=details.days;
			break;
	}
	if(wait_list[department-1].head==NULL){
		wait_list[department-1].head=p;
		wait_list[department-1].tail=p;
		p->next=NULL;
	}
	else{
		wait_list[department-1].tail->next=p;
		wait_list[department-1].tail=p;
		p->next=NULL;
	}
}

//Prints all the names in the list with the corresponding department and info
void list(){
	NODE *p;
	int i, x;
	x=1;
	printf("\nWAITING LIST\n");
	printf("Name\t\tDepartment\t\tDetails\n");
	printf("-------------------------------------------------------\n");
	for(i=0;i<4;i++){
		p=wait_list[i].head;
		while(p!=NULL){
			printf("%s\t\t%d\t\t", p->name, p->department);
			switch(p->department){
				case 1:
					printf("Temperature: %f\n", p->details.fever);
					break;
				case 2:
					printf("Pain: %s\n", p->details.pain);
					break;
				case 3:
				case 4:
					printf("Days: %d\n", p->details.days);
					break;
			}
			p=p->next;
			x++;
		}
	}
	printf("-------------------------------------------------------\n");
}

//Removes the oldest name from a given department
void rmv(){
	int department;
	printf("Which department is this for?: ");
	department=get_department();
	NODE *p;
	p=wait_list[department-1].head;
	if(p==NULL){
		printf("No one from that department is in the list.\n");
		return;
	}
	else{
		wait_list[department-1].head=p->next;
		free(p);
	}
}

//Shows the entries for a given department
void show_department(){
	NODE *p;
	int i, department, x;
	x=1;
	printf("Which department do you want to show?: ");
	department=get_department();
	printf("\nDEPARTMENT %d\n", department);
	printf("Name\t\tDetails");
	printf("\n---------------------------------------\n");
	p=wait_list[department-1].head;
	while(p!=NULL){
		printf("%s\t\t", p->name);
		switch(department){
			case 1:
				printf("Temperature: %f\n", p->details.fever);
				break;
			case 2:
				printf("Pain: %s\n", p->details.pain);
				break;
			case 3:
			case 4:
				printf("Days: %d\n", p->details.days);
				break;
		}
		x++;
		p=p->next;
	}
	printf("---------------------------------------\n");
}

//Shows the corresponding info for a given person
void show_info(){
	int x, i;
	char name[10];
	NODE *p;
	printf("Who is this for?: ");
	scanf("%s", name);
	for(i=0; i<4; i++){
		p=wait_list[i].head;
		while(p!=NULL){
			x=strcmp(p->name, name);
			if(x==0){
				printf("INFO");
				printf("--------------\n");
				printf("NAME: %s\nDEPARTMENT: %d\n", p->name, p->department);
				switch(p->department){
					case 1:
						printf("TEMPERATURE: %f\n", p->details.fever);
						break;
					case 2:
						printf("PAIN: %s\n", p->details.pain);
						break;
					case 3:
					case 4:
						printf("DAYS: %d\n", p->details.days);
						break;
				}
				printf("--------------\n");
				return;
			}
			p=p->next;
		}
	}
	printf("Sorry that name is not in the list.\n");
}

//Moves a name from one department to another
void change_department(){
	int department, i, q;
	q=0;
	char name[10];
	union info details;
        NODE *p, *temp;
        printf("Who is this for?: ");
        scanf("%s", name);
	for(i=0; i<4; i++){
		p=temp=wait_list[i].head;
		if(p!=NULL){
			if(strcmp(p->name,name)==0){
				wait_list[i].head=p->next;
				free(p);
				q=1;
				break;
			}
			p=p->next;
			while(p!=NULL){
				if(strcmp(p->name,name)==0){
					temp->next=p->next;
					free(p);
					q=1;
					i=4;
					break;
				}
				p=p->next;
				temp=temp->next;
			}
		}
	}
	if(q==0){
		printf("Sorry that name is not in the list.\n");
		return;
	}
	printf("What department do you want to move into?: ");
	department = get_department();
        details=get_details(department);
        insert(name, department, details);
}

//Imports from bianary file
void read_bfile(char *fname){
	FILE *fp;
	NODE p;
	fp = fopen(fname, "rb");
	if(fp==NULL){
		printf("Waiting list is empty\n");
		return;
	}
	printf("\nWAITING LIST\n");
	printf("Name\t\tDepartment\t\tDetails\n");
	printf("-------------------------------------------------------\n");
	while(fread(&p, sizeof(NODE), 1, fp)>0){
		printf("%s\t\t%d\t\t", p.name, p.department);
		switch(p.department){
			case 1:
				printf("Temperature: %f\n", p.details.fever);
				break;
			case 2:
				printf("Pain: %s\n", p.details.pain);
				break;
			case 3:
			case 4:
				printf("Days: %d\n", p.details.days);
				break;
		}
	}
	printf("-------------------------------------------------------\n");
	fclose(fp);
	return;
}

//Imports text file
void read_tfile(char *fname, int key){
	FILE *fp;
	char name[10], buffer[50];
	int i, size, department;
	union info details;
	fp = fopen(fname, "r");
	if(fp==NULL)
		return;
	fseek(fp, 26, SEEK_SET);
	while(fgets(buffer, 50, fp)!=NULL){
		for(i=0;buffer[i]!='\n';i++){
			buffer[i]=((buffer[i])^(key));
		}
		sscanf(buffer, "%s\t%d", name, &department);
		switch(department){
			case 1:
				sscanf(buffer, "%s\t%d\t%f\n", name, &department, &details.fever);
				insert(name, department, details);
				break;
			case 2:
				sscanf(buffer, "%s\t%d\t%s\n", name, &department, details.pain);
				insert(name, department, details);
				break;
			case 3:
			case 4:
				sscanf(buffer, "%s\t%d\t%d\n", name, &department, &details.days);
				insert(name, department, details);
				break;
		}
	}
	fclose(fp);
	return;
}

//Saves list to text file
void write_tfile(char *fname, int key){
	FILE *fp;
	NODE *p;
	int i, j;
	char buffer[100];
	fp = fopen(fname, "w");
	fprintf(fp, "Name\t\tDepartment\t\tDetails\n");
	for(i=0;i<4;i++){
		p=wait_list[i].head;
		while(p!=NULL){
			switch(p->department){
				case 1:
					sprintf(buffer, "%s\t%d\t%f\n",p->name,p->department,p->details.fever);
					break;
				case 2:
					sprintf(buffer, "%s\t%d\t%s\n",p->name,p->department,p->details.pain);
					break;
				case 3:
				case 4:
					sprintf(buffer, "%s\t%d\t%d\n",p->name,p->department,p->details.days);
					break;
			}
			for(j=0;buffer[j]!='\n';j++)
				buffer[j]=((buffer[j])^(key));
			fprintf(fp, "%s", buffer);
			p=p->next;
		}
	}
	fclose(fp);
	return;
}



void *auto_save(void *arg){
	FILE *fp;       
	NODE *p;
	int i;
	while(1){
		sleep(15);
		pthread_mutex_lock(&mutex);
		fp = fopen(arg, "wb");
		for(i=0;i<4;i++){
			p=wait_list[i].head;
			while(p!=NULL){
				fwrite(p, sizeof(NODE), 1, fp);
				p=p->next;
			}
		}
		fclose(fp);
		pthread_mutex_unlock(&mutex);
	}
}

int main(int arg, char *argu[]){
	int i, x, department, key;
	char name[10];
	union info details;
	for(i=0;i<4;i++){
		wait_list[i].head=NULL;
		wait_list[i].tail=NULL;
	}
	if(arg!=4){
		printf("ERROR: NOT ENEOUGH ARGUMENTS\n");
		return 0;
	}
	key=atoi(argu[3]);
	read_tfile(argu[1],key);
	printf("END\n");
	pthread_t thr;
	pthread_mutex_init (&mutex, NULL);
	pthread_create (&thr, NULL, auto_save, (void *)argu[2]);
	while(1){
		printf("\t\t\tOptions\n");
		printf("-----------------------------------------------------------------------------\n");
		printf("1:Add Name\t\t2:View List\t\t3:Remove Name from Department\n");
		printf("4:Show Department\t5:Person Info\t\t");
		printf("6:Change Persons Department\n7:Read Bianary File\t\t0:Save and Quit\n");
		printf("Entry: ");
		scanf("%d", &x);
		if(x==0)
			break;
		switch(x){
			case 1:
				pthread_mutex_lock(&mutex);
				printf ("What is your name?: ");
				scanf ("%s", name);
				if(check_duplicate(name)==0){
					printf("Sorry that name is already in the list.\n");
					pthread_mutex_unlock(&mutex);
					break;
				}
				printf("What department is this for?: ");
				department=get_department();
				details=get_details(department);
				insert(name, department, details);
				pthread_mutex_unlock(&mutex);
				break;
			case 2:
				list();
				break;
			case 3:
				pthread_mutex_lock(&mutex);
				rmv();
				pthread_mutex_unlock(&mutex);
				break;
			case 4:
				show_department();
				break;
			case 5:
				show_info();
				break;
			case 6:
				pthread_mutex_lock(&mutex);
				change_department();
				pthread_mutex_unlock(&mutex);
				break;
			case 7:
				pthread_mutex_lock(&mutex);
				read_bfile(argu[2]);
				pthread_mutex_unlock(&mutex);
				break;
		}
	}
	pthread_mutex_lock(&mutex);
	write_tfile(argu[1],key);
}
