//HASHTABLE CODE
struct nlist { /* table entry: */
    double start_time; /* starttime */
    double finish_time;/* finishtime */
    int index; /* index // this is the line index in the input text file */
    int pid; /* pid  // the process id. you can use the pid result of wait to lookup in the hashtable */
    char *command; // command. This is good to store for when you decide to restart a command */
    struct nlist *next; /* next entry in chain */
};

 #define HASHSIZE 101
 static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/* This is the hash function: form hash value for string s */
/* TODO change to: unsigned hash(int pid) */
/* TODO modify to hash by pid . /*
/* You can use a simple hash function: pid % HASHSIZE */
unsigned hash(int pid)
{
//    unsigned hashval;
//     for (hashval = 0; *s != '\0'; s++)
//       hashval = *s + 31 * hashval;
    return ((unsigned)(pid % HASHSIZE));
}

/* lookup: look for s in hashtab */
/* TODO change to lookup by pid: struct nlist *lookup(int pid) */
/* TODO modify to search by pid, you won't need strcmp anymore */
/* This is traversing the linked list under a slot of the hash table. The array position to look in is returned by the hash function */
struct nlist *lookup(int pid)
{
    struct nlist *np;
    for (np = hashtab[hash(pid)]; np != NULL; np = np->next)
        if (pid == np->pid)
          return np; /* found */
    return NULL; /* not found */
}


char *strdup2(char *);


/* insert: put (name, defn) in hashtab */
/* TODO: change this to insert in hash table the info for a new pid and its command */
/* TODO: change signature to: struct nlist *insert(char *command, int pid, int index). */
/* This insert returns a nlist node. Thus when you call insert in your main function  */
/* you will save the returned nlist node in a variable (mynode). */
/* Then you can set the starttime and finishtime from your main function: */
/* mynode->starttime = starttime; mynode->finishtime = finishtime; */
// struct nlist *insert(char *name, char *defn)
// {
//     struct nlist *np;
//     unsigned hashval;
//     //TODO change to lookup by pid. There are 2 cases:
//     if ((np = lookup(name)) == NULL) { /* case 1: the pid is not found, so you have to create it with malloc. Then you want to set the pid, command and index */
//         np = (struct nlist *) malloc(sizeof(*np));
//         if (np == NULL || (np->name = strdup(name)) == NULL)
//           return NULL;
//         hashval = hash(name);
//         if ((np->defn = strdup(defn)) == NULL)
//            return NULL;
//         np->next = hashtab[hashval];
//         hashtab[hashval] = np;
//     } else { } /* case 2: the pid is already there in the hashslot, i.e. lookup found the pid. In this case you can either do nothing, or you may want to set again the command  and index (depends on your implementation). */
//         //free((void *) np->defn); /*free previous defn */
//     return np;
// }

struct nlist *insert(char *command, int pid, int index) {

    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(pid)) == NULL) {       //linkedlist not found
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL || (np->command = strdup2(command)) == NULL) {
            return NULL;
        }
        np->pid = pid;
        np->index = index;
        hashval = hash(pid);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    //free maybe?
    return np;
}

/** You might need to duplicate the command string to ensure you don't overwrite the previous command each time a new line is read from the input file.
Or you might not need to duplicate it. It depends on your implementation. **/
char *strdup2(char *s) /* make a duplicate of s */
{
    char *p;
    p = (char *) malloc(strlen(s)+1); /* +1 for ’\0’ */
    if (p != NULL)
       strcpy(p, s);
    return p;
}
//End of HASHTABLE CODE

#define MAX_COMMAND_LENGTH 30
char line[MAX_COMMAND_LENGTH + 1];
char *args[100];    // assume 100 commands max
int index = 0;
//put commands in a data structure (e.g. hash table), which is used for recording for each exec
// the start time, finish time, index (line number in the commands file), pid and command.
//see the read_parse_file.c I provided for parsing each command, then
//save each cmd in an entry of the hashtable.

while ((linesize = getline(&line, &len, fp)) >= 0) {

    index++; // this index is the line number in the commands file.

    //removes new line character from line
        line[strcspn(line, "\n")] = 0;

        int arg_count = 0;
        args[arg_count++] = strtok(line, " ");
        //Separates words by space and places word pointers in array
        while ( args[arg_count] != NULL) {
            args[arg_count] = strtok(NULL, " ");
            arg_count++;
        }
        args[arg_count] = NULL;

    //---------------------------
    //fork the commands and record the start times

    //save the startime!
    // starttime = current time

pid = fork();

  if (pid < 0) {

         fprintf(stderr, "error forking");
       exit(2);
    } else if (pid == 0) { /*child */
//See shell1_execvp.c for execvp usage
 execvp(.... command);  /*executes the command in the specific node */

} else if (pid > 0) {  /* parent goes to the next node */

        //insert the new pid into the hash table
        // entry_new = call hash insert(pid);
        //record the new starttime!
        // entry_new.starttime = starttime
        // entry_new.command = line
        // entry_new.index = index

int fdout = open("%d.out", pid);
int fderr = open("%d.err", pid);
fprintf(fdout, "Starting command INDEX %d: child PID %d of parent PPID %d.\n", entry.index, pid, getpid() );

}
} /*end of while loop*/


//---------------------------
//Final while loop: waits until anything has completed,
//this will exit (wait returns -1)
//when there is no more child process. Then your parent process exits.

while((pid = wait(&status)) >= 0) {
    if(pid > 0) {
        //finishtime = get the finish time current time
        //search your hashtable for the entry that corresponds to pid
        //The function lookup was provided in the hashtable code
        //entry=hash lookup(pid)

        //signal handling
        int fdout = open("%d.out", pid);
        int fderr = open("%d.err", pid);

        fprintf(stderr, "Process with PID %d terminated.\n", pid);
        if (WIFEXITED(status)) {
            fprintf(fderr, "Child %d terminated normally with exit code: %d\n",
                   pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            fprintf(fderr, "Child %d terminated abnormally with signal number: %d\n",
                   pid, WTERMSIG(status));   }

        //
        //to compute the elapsed time you subtract
        //elapsedtime = finishtime - entry.start_time
        //decide if you will restart
        //if (elapsedtime > 2) {
        //save the startime!
        // starttime = current time
        // pid = fork();
        // if (pid < 0) //error
        // else if (pid == 0) { //child
        // See shell1_execvp.c for execvp usage
        // execvp(entry.command);
        // } else if (pid > 0) {
         //insert the new pid into the hash table
        // entry_new = call hash insert(pid);
        //record the new starttime!
        // entry_new.starttime = new starttime
        // entry_new.index = entry.index
        // entry_new.command = entry.command
        // }

    }
}
