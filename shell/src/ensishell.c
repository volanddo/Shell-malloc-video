/*****************************************************
 * Copyright Grégory Mounié 2008-2015                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "variante.h"
#include "readcmd.h"

#define PIDLISTLEN 50

#ifndef VARIANTE
#error "Variante non défini !!"
#endif

/* Guile (1.8 and 2.0) is auto-detected by cmake */
/* To disable Scheme interpreter (Guile support), comment the
 * following lines.  You may also have to comment related pkg-config
 * lines in CMakeLists.txt.
 */

struct process_in_bg **glob_ptr_to_struct;
int glob_pidList[PIDLISTLEN];


void terminate(char *line) {
#if USE_GNU_READLINE == 1
	/* rl_clear_history() does not exist yet in centOS 6 */
	clear_history();
#endif
	if (line)
	  free(line);
	printf("exit\n");
	exit(0);
}

/*******************************    Fonctions auxiliaires   *******************************************/
size_t give_nb_cmd_and_print_it(char ***seq) {
	int i;
	for (i=0; seq[i]!=0; i++) {
	}
	return i;
}

void add_pid_in_pidList(int *pidList, pid_t pid) {
	int i = 0;
	while (i < PIDLISTLEN && pidList[i] != -2) {
		i++;
	}
	if (i == PIDLISTLEN) perror(0);
	pidList[i] = pid;

}

void add_process_in_bg(struct process_in_bg **ptr, char ***seq, pid_t pid, int *pidList) {
	struct process_in_bg *ptr_to_add = malloc(sizeof(struct process_in_bg));

	int numberofcmd = 0;
	for (int j=0; seq[j]!=0; j++) {
		numberofcmd++;
	}
	ptr_to_add->seq = malloc((numberofcmd+1)*sizeof(char**)); // on va copier seq dans ce pointeur

	for (int i=0; seq[i]!=0; i++) {

		char **cmd = seq[i];

		int strlencmd = 0; // permet juste de savoir la taille d'une commande
		for (int j=0; cmd[j]!=0; j++) {
			strlencmd++;
		}

		ptr_to_add->seq[i] = malloc((strlencmd+1) * sizeof(char*)); // on alloue la place pour une commande

		for (int j=0; cmd[j]!=0; j++) {
			ptr_to_add->seq[i][j] = malloc(1000 * sizeof(char));
			strcpy(ptr_to_add->seq[i][j], cmd[j]);
		}
		ptr_to_add->seq[i][strlencmd] = NULL;
	}
	ptr_to_add->seq[numberofcmd] = 0;
	ptr_to_add->pid = pid;
	ptr_to_add->next = NULL;

	if (*ptr == NULL) { // si aucune cellule dans la structure
		*ptr = ptr_to_add;
		add_pid_in_pidList(pidList, pid);
		return;
	}

	struct process_in_bg *cellule = *ptr;
	while (cellule->next != NULL) { // au moins une cellule dans la structure
		cellule = cellule->next;
	}
	cellule->next = ptr_to_add;
	add_pid_in_pidList(pidList, pid);
	return;
}

void remove_process_in_bg(struct process_in_bg **ptr, pid_t pid) { // !!!!!!! à faire : free le ptr->seq aussi !!!!!!!!!
	struct process_in_bg *cellule = *ptr;
	if (cellule == NULL) return;
	if (cellule->pid == pid) {
		*ptr = cellule->next;
		free(cellule);
		return;
	}
	while (cellule->next->pid != pid) {
		cellule = cellule -> next;
	}
	struct process_in_bg *cellule_to_destroy = cellule->next;
	cellule->next = cellule_to_destroy->next;
	free(cellule_to_destroy);
	return;
}

void remove_finished_bg_process(int *pidList, struct process_in_bg **ptr) {
	int i = 0;
	while (i < PIDLISTLEN) {
		if(pidList[i] != -2 && waitpid(pidList[i], NULL, WNOHANG) != 0) { // le processus de pid_t pid a fini
    		remove_process_in_bg(ptr, pidList[i]);
			pidList[i] = -2;
		} 
		i++;
	}
}

int jobsFunction(char ***seq, struct process_in_bg **ptr) {
	for (int k = 0; seq[k] != NULL; k++) {
		if ( strcmp((seq[k])[0], "jobs") == 0 ) {
			struct process_in_bg *cellule = *ptr;
			char str1[30] = {0};
			char displayCommand[1000] = {0};
			memset(str1, 0, strlen(str1));
			memset(displayCommand, 0, strlen(displayCommand));
			strcat(displayCommand, "Processus actif en arrière-plan : # ");

			while (cellule != NULL) {
				char ***seq = cellule->seq;
				memset(str1, 0, strlen(str1));
				snprintf(str1, 30, "pid : %i, commande : ", cellule->pid);
				strcat(displayCommand, str1);
				for ( int k = 0; seq[k] != 0; k++ ) {
					memset(str1, 0, strlen(str1));
					char **cmd = seq[k];
					for (int i = 0; cmd[i] != 0; i++) {
						memset(str1, 0, strlen(str1));
						snprintf(str1, 30, "%s ", cmd[i]);
						strcat(displayCommand, str1);
					}
				}
				strcat(displayCommand, "# ");
				cellule = cellule->next;
			}
			printf("%s\n", displayCommand);
			return 1;
		}
	}
	return 0;
}

void print_in_out_bg(struct cmdline *l) {
	if (l->in) printf("Input file : %s\n", l->in);
	if (l->out) printf("Output file : %s\n", l->out);
	if (l->bg) printf("Will run in background (&)\n");
}

int isThereError(int x) {
	if (x == -1) {
		perror(0);
		return 1;
	}
	return 0;
}

int exec_pipe_cmd(int tuyau_lecture, int tuyau_ecriture, struct cmdline *l, int index) {
	int pid;
	pid = fork();
	if (isThereError(pid)) return -1;
	if (pid == 0) { 
		if (isThereError(dup2(tuyau_lecture, 0))) exit(errno);
		if (isThereError(dup2(tuyau_ecriture, 1))) exit(errno);
		execvp(l->seq[index][0], l->seq[index]); 
		exit(errno);
	}
	return pid;
}

int exec_pipe(struct cmdline *l, struct process_in_bg **ptr, int numberOfCommands, int *pidList) {
	int pid;
	pid = fork();
	if (isThereError(pid)) return -1;
	if (pid == 0) {
		pid_t *pid_tab = calloc(sizeof(pid_t),numberOfCommands);
		int tuyau[2];
		if (isThereError(pipe(tuyau))) return -1;
		int fd_in;
		if (l->in) { // cas où on a < 
			fd_in = open(l->in, O_RDONLY); // fd_in pointe vers le fichier ouvert l->in
			if (isThereError(fd_in)) return -1;
		}
		else {
			fd_in = 0;
		}
		int fd_out = tuyau[1];
		for ( int i = 0 ; i < numberOfCommands; i++ ) {
			pid = exec_pipe_cmd(fd_in, fd_out, l, i);
			if (isThereError(pid)) exit(errno);
			if (i != numberOfCommands-1 || l->out) close(fd_out); // je n'ai plus besoin d'écrire dans l'ancien tuyau donc je ferme tuyau[1] sauf si c'est la derniere cmd et que l->out = 0
			if (i != 0 || l->in) close(fd_in); // si on est sur la première commande pas la peine de fermer fd_in si il n'y a pas de l->in car c'est stdin sinon je ferme car plus besoin
			fd_in = tuyau[0]; // je met fd_in sur tuyau[0] pour que la prochaine commande reçoive ce que j'ai écrit
			if (i <  numberOfCommands - 2 ) { // pas la peine de créer de tuyau si la condition est pas vérifiée
				if (isThereError(pipe(tuyau))) exit(errno);
				fd_out = tuyau[1]; // j'écris sur l'extrémité écriture du prochaine tuyau
			}
			else {
				if (l->out) { // cas où on a >
					fd_out = open(l->out, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU); // fd_out pointe vers le fichier l->out. Le crée si il n'existe pas
					if (isThereError(fd_out)) exit(errno);
				}
				else {
					fd_out = 1; // on est sur la dernière commande donc on écrira sur stdin si l->out = 0
				}
			}
			pid_tab[i] = pid;
		}
		for (int i = 0; i < numberOfCommands; i++){
			waitpid(pid_tab[i], NULL, 0); // !!!!! A IMPLEMENTER : RECOLTER TOUS LES PID DES CMD ENCORE EN BG ET LES ATTENDRE DANS LE PROCESSUS PERE SI l->bg = 0 !!!!!!!!!!
		}
		kill(getpid(), SIGTERM); // on tue ce processus qui ne servait qu'à faire le pipe
	}
	(l->bg) ? add_process_in_bg(ptr, l->seq, pid, pidList) : waitpid(pid, NULL, 0); // si on tourne pas en background, on attends que le pipe se finisse
	return 0;
}

int exec_cmd(struct cmdline *l, struct process_in_bg **ptr, int *pidList) {
	int pid;
	int fd_in = -2; // on les initialise à -2 pour tester après si l->in et l->out sont NULL ou pas
	int fd_out = -2;
	if (l->in) { // cas où on a < 
		fd_in = open(l->in, O_RDONLY); // fd_in pointe vers le fichier ouvert l->in
		if (isThereError(fd_in)) return -1;
	}
	if (l->out) { // cas où on a >
		fd_out = open(l->out, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU); // fd_out pointe vers le fichier l->out. Le crée si il n'existe pas
		if (isThereError(fd_out)) return -1;
	}
	pid = fork(); // pid vaut la valeur du global pid du fils
	if (isThereError(pid)) return -1;
	if (pid == 0) { // c'est le fils
		int saved_stdout = dup(1);
		if (fd_in != -2) {
			if (isThereError(dup2(fd_in, 0))) exit(errno);
			close(fd_in);
		}
		if (fd_out != -2) {
			if (isThereError(dup2(fd_out, 1))) exit(errno);
			close(fd_out);
		}
		execvp((l->seq[0])[0], l->seq[0]);
		dup2(saved_stdout, 1);
		printf("Unknown command\n");
		exit(errno); // in case the execvp failed
	}
	(l->bg) ? add_process_in_bg(ptr, l->seq, pid, pidList) : waitpid(pid, NULL, 0); // vérifie si on fait tourner en background ou pas
	return 0;
}
/******************************************************************************************************/
void execute_line(struct cmdline *l){
	/* If input stream closed, normal termination */
		if (!l) terminate(0);
		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			return;
		}

		// !!!!!!  ( amélioration à faire : ne pas faire continue mais plutôt exécuter les autres commandes )
		if (jobsFunction(l->seq, glob_ptr_to_struct)) return; // si il y a eu jobs on retourne au début du while

		/* Display each command of the pipe, output and inputfiles, running in background or not */
		size_t numberOfCommands = give_nb_cmd_and_print_it(l->seq);
		print_in_out_bg(l);

		switch(numberOfCommands) {
			case 0: // Aucune commande
				break;
			case 1: // On a que une commande, pas de pipe
				;
				if (isThereError(exec_cmd(l, glob_ptr_to_struct, glob_pidList))) return;
				break;
			default: // On a au minimum 1 pipe.
				;
				if (isThereError(exec_pipe(l, glob_ptr_to_struct, numberOfCommands, glob_pidList))) return;
				break;
		}
		//printf("\n"); // permet de flush le buffer
}

#if USE_GUILE == 1
#include <libguile.h>

int question6_executer(char *line)
{
	/* Question 6: Insert your code to execute the command line
	 * identically to the standard execution scheme:
	 * parsecmd, then fork+execvp, for a single command.
	 * pipe and i/o redirection are not required.
	 */
	
	struct cmdline * l = parsecmd( & line);
	execute_line(l);
	/* Remove this line when using parsecmd as it will free it */
	free(line);
	
	return 0;
}

SCM executer_wrapper(SCM x)
{
        return scm_from_int(question6_executer(scm_to_locale_stringn(x, 0)));
}
#endif


int main() {
    printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);

#if USE_GUILE == 1
        scm_init_guile();
        /* register "executer" function in scheme */
        scm_c_define_gsubr("executer", 1, 0, 0, executer_wrapper);
#endif

	/* Warning : Bien penser à free le pointeur car je ne l'ai pas encore fait  */
	glob_ptr_to_struct = malloc(sizeof(struct process_in_bg*)); // pointeur pointant sur le premier process en background, sur NULL si aucun
	*glob_ptr_to_struct = NULL; // il n'y a pas de cellule au début
	for (int i = 0; i<PIDLISTLEN; i++) { // on initialise la liste avec la valeur -2 pour dire qu'il n'y en a aucun
		glob_pidList[i] = -2;
	}
	while (1) {
		struct cmdline *l;
		char *line=0;
		char *prompt = "ensishell>";

		/* Readline use some internal memory structure that
		   can not be cleaned at the end of the program. Thus
		   one memory leak per command seems unavoidable yet */
		line = readline(prompt);
		if (line == 0 || ! strncmp(line,"exit", 4)) {
			terminate(line);
		}

#if USE_GNU_READLINE == 1
		add_history(line);
#endif


#if USE_GUILE == 1
		/* The line is a scheme command */
		if (line[0] == '(') {
			char catchligne[strlen(line) + 256];
			sprintf(catchligne, "(catch #t (lambda () %s) (lambda (key . parameters) (display \"mauvaise expression/bug en scheme\n\")))", line);
			scm_eval_string(scm_from_locale_string(catchligne));
			free(line);
                        continue;
                }
#endif

		remove_finished_bg_process(glob_pidList, glob_ptr_to_struct); // avant de parser, on regarde si il n'y a pas des processus qui ont finis en arrière plan
		/* parsecmd free line and set it up to 0 */
		l = parsecmd( & line);
		execute_line(l);
	}
}
