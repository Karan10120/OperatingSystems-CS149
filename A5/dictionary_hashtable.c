#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * The reason for using a hashtable is to store the commands in hash slots by their pids.
 * This way you can lookup a command by a pid and retrieve all the info you need:
 *  command, index (this index means the line number in the text file you read),
 *  start time, and anything else you might need.
 *
 * A hashtable (as you might remember from CS146)
 * has slots and each slot contains a linked list of nodes 
 * (these are the entries that contain all the command info).
 * Thus the hashtable (see hashtab array variable below) is 
 * implemented as an array of nlists. Each array position is a 
 * slot and a linked list of nlist nodes starts at each array slot. 
 * Each array position is a hahstable slot.
 *
 * You find the hashtable slot for a pid by using a hash function, 
 * which will map the pid to a hashtable slot (array position).
 *
 * You can copy this entire code directly in your .c code. No need to have
 * many files. 
 *
 *
 * This nlist is a node, which stores one command's info in the hashtable.
 * TODO: You will need to adopt this code a little.
 *
 * The char *name and char *defn you can remove.
 * The nlist *next field is a pointer to the next node in the linked list.  
 * There is one hashtable slot in each array position, 
 * consequently there is one linked list of nlists under a hashtable slot. 
 */



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

void printnode(int pid) {
    printf("PID: %d\n", lookup(pid)->pid);
    printf("Command: %s\n", lookup(pid)->command);
    printf("Index: %d\n", lookup(pid)->index);
}

void printnodes(struct nlist *node) {
    printf("PID: %d, Command: %s\n", node->pid, node->command);
}

void printhashtable() {
    for (int i = 0; i < HASHSIZE; i++) {
        struct nlist *np = hashtab[i];
        while (np != NULL) {
            printnodes(np);
            np = np->next;
        }
    }
}

void main() {


    //static struct nlist *hashtab[HASHSIZE]; /* pointer table */
    insert("touch", 4942, 0);
    insert("sleep", 5962, 1);
    insert("wc", 9293, 2);
    insert("something", 3597, 3);

    printf("Print Nodes:\n");
    printnode(4942);
    printnode(5962);
    printnode(9293);
    printnode(3597);

    printf("Print Hashtable:\n");
    printhashtable();


}
