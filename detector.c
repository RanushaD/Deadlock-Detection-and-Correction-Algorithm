/*
 ============================================================================
 Name        : detector.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add2array(int iP, int total_process, int order_array[], int terminated[], int finish[]){
	if (finish[iP] == 1 || finish[iP] == 2){
		int i = 0;
		while(i < total_process && order_array[i] != -1){
			i = i + 1;
		}
		order_array[i] = iP+1;
	}
	else if (finish[iP] == -1){
		int j = 0;
		while(j < total_process && terminated[j] != -1){
			j = j + 1;
		}
		terminated[j] = iP+1;
	}
}

void terminate_display(int total_process, int terminated[], int finish[]){
	int i = 0;

	while (finish[i] != 2 && i < total_process){
		printf("%d ", i + 1);
		i = i + 1;
	}
	printf("\n");
	int j = 0;
	while(j < total_process && terminated[j] != -1){
		printf("%d ", terminated[j]);
		j = j + 1;
	}
}

void complete_display(int total_process, int order_array[]){
	int i = 0;
	while(i < total_process && order_array[i] != -1){
		printf("%d ", order_array[i]);
		i = i + 1;
	}
}

int canRun(int total_resource, int total_process, int request[][total_resource], int available[], int runState[], int finish[]){
	int i = 0, run = 0;
	while(i < total_process){
		int j = 0, canRun = 0;
		while(j < total_resource && canRun != total_resource && finish[i] == 0){
			if(request[i][j] <= available[j]){
				canRun = canRun + 1;
			}
			j = j + 1;
		}
		if (canRun == total_resource){
			runState[i] = 1;//process(i) is canRun
			run = 1;
		}
		else {
		}
		i = i + 1;
	}
	return run;
}

int allDone(int total_process, int finish[]){
	int i = 0, done = 0, complete = 0;
	while (i < total_process && complete != total_process ){
		if (finish[i] == 1 || finish[i] == -1 || finish[i] == 2){
			complete = complete + 1;
		}
		i = i + 1;
	}
	if (complete == total_process){
		done = 1;
	}
	return done;
}

void updateAvailability(int i, int total_resource, int available[], int allocation[][total_resource]){
	for(int j = 0; j < total_resource; j++){
		//update availability
	available[j] = available[j] + allocation[i][j];
	}
}

int terminate(int total_resource, int finish[], int available[], int allocation[][total_resource]){
	int i = 0; // smallest deadlock
	while (finish[i] != 0){
		i = i + 1;
	}
	finish[i] = -1; //process(i) terminated
	updateAvailability(i, total_resource, available, allocation);
	return i;
}

int main(void) {
	setvbuf(stdout, NULL, _IONBF,0); //buffer
	char input[200];
	int total_process;
	int total_resource;

	fgets(input, sizeof(input), stdin);

	total_process = strtol(strtok(input, " "), NULL, 0);
	total_resource = strtol(strtok(NULL, " "), NULL, 0);

	int request[total_process][total_resource];
	int allocation[total_process][total_resource];
	int available[total_resource];
	int finish[total_process];
	int runState[total_process];
	int order_array[total_process];
	int terminated[total_process];
	int termination = 0;
	int line = 0;

	for (int i = 0; i < total_process; i++){
		order_array[i] = -1; //  initialize
	}

	for (int i = 0; i < total_process; i++){
		terminated[i] = -1; //  initialize
	}

	while (fgets(input, sizeof(input), stdin) != NULL && line < (2*total_process+1)) {
		if (strcmp(input, "\n\0") != 0) {
			int count = strtol(strtok(input, " "), NULL, 0);
			if (line < total_process){
				request[line][0] = count;
//				printf("R Count %d\n", count);
				} else if  (line < 2*total_process){
					int index = line - total_process;
					allocation[index][0] = count;
//					printf("AL Count %d\n", count);

				} else{
					available[0] = count;
//					printf("AV first Count %d\n", count);
				}
			int i = 0, iR = 1, iR2 = 1;
			while(i < total_resource - 1 ){
				count = strtol(strtok(NULL, " "), NULL, 0);
//				printf("Count %d\n", count);
				if (line < total_process){
					request[line][iR] = count;
				} else if (line < 2*total_process){
					int index = line - total_process;
					allocation[index][iR] = count;
				} else {
						available[iR2] = count;
						iR2 = iR2 + 1;
				}
				iR = iR + 1;
				i = i + 1;
			}
		}
		line = line + 1;
	}

	for (int i = 0; i < total_process; i++){
		finish[i] = 0; // 0 = false
	}

	for (int i = 0; i < total_process; i++){
		runState[i] = 0; // 0 = false
	}

	int run = canRun(total_resource, total_process, request, available, runState, finish);
	while(run == 1){
		int i = 0;
		while(i < total_process){
			if(runState[i] == 1){
				updateAvailability(i, total_resource, available, allocation);
				runState[i] = -1; // process(i) Ran
				finish[i] = 2;
				add2array(i, total_process, order_array, terminated, finish);
			}
			i = i + 1;
		}
		run = canRun(total_resource, total_process, request, available, runState, finish);
	}

	int done = allDone(total_process, finish);// check whether all process are finished
	while (done == 0){

		if (run == 0){ // deadlock is detected
			//terminate smallest deadlock
			int i = terminate( total_resource, finish, available, allocation);
			termination = 1;
			add2array(i, total_process, order_array, terminated, finish);
		}
		if (run == 1){
			int j = 0;
			while(j < total_process){
				if(runState[j] == 1){
					updateAvailability(j, total_resource, available, allocation);
					runState[j] = -1; // process(j) Ran
					finish[j] = 1; // process(j) Done
					add2array(j, total_process, order_array, terminated, finish);
				}
				j = j + 1;
			}
		}
		run = canRun(total_resource, total_process, request, available, runState, finish);
		done = allDone(total_process, finish);
	}

	if (termination == 1){
		terminate_display(total_process, terminated, finish);
		printf("\n");
	}
	complete_display(total_process, order_array);
//
	for (int i = 0; i< total_process; i++){
		for (int j = 0; j < total_resource; j++){
			printf("%d ", request[i][j]);
		}
		printf("\n");
	}
}
