#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct ContactNode_struct ContactNode;
struct ContactNode_struct {
   char contactName[50];
   char contactPhoneNumber[50];
   struct ContactNode* nextNodePtr;
};

void InitializeContactNode(char* name, char* phone, ContactNode* node){
   name[strcspn(name, "\n")] = '\0';
   phone[strcspn(phone, "\n")] = '\0';
   strcpy(node->contactName, name);
   strcpy(node->contactPhoneNumber, phone);
   node->nextNodePtr = NULL;
}

char* GetName(ContactNode* contact) {
   return (contact->contactName);
}

char* GetPhoneNumber(ContactNode* contact) {
   return (contact->contactPhoneNumber);
}

void InsertAfter(ContactNode* prevNode, ContactNode* nextNode){
   nextNode->nextNodePtr = prevNode->nextNodePtr;
   prevNode->nextNodePtr = nextNode;
}   

ContactNode* GetNext(ContactNode* contact) {
   return (contact->nextNodePtr);
}

void PrintContactNode(ContactNode* contact){
   printf("Name: %s\n", GetName(contact));
   printf("Phone number: %s\n", GetPhoneNumber(contact));
}

int main(void) {
   
   ContactNode* headNode = (ContactNode*) malloc(sizeof(ContactNode));
   ContactNode* secondNode = (ContactNode*) malloc(sizeof(ContactNode));
   ContactNode* thirdNode = (ContactNode*) malloc(sizeof(ContactNode));
   
   char nameInput[50];
   char phoneInput[50];
   
   fgets(nameInput, 64, stdin);
   fgets(phoneInput, 64, stdin);
   InitializeContactNode(nameInput, phoneInput, headNode);
   printf("Person 1: %s, %s\n", GetName(headNode), GetPhoneNumber(headNode));
   
   fgets(nameInput, 64, stdin);
   fgets(phoneInput, 64, stdin);
   InitializeContactNode(nameInput, phoneInput, secondNode);
   InsertAfter(headNode, secondNode);
   printf("Person 2: %s, %s\n", GetName(secondNode), GetPhoneNumber(secondNode));
   
   fgets(nameInput, 64, stdin);
   fgets(phoneInput, 64, stdin);
   InitializeContactNode(nameInput, phoneInput, thirdNode);
   InsertAfter(secondNode, thirdNode);
   printf("Person 3: %s, %s\n", GetName(thirdNode), GetPhoneNumber(thirdNode));
   
   ContactNode* current = headNode;
   printf("\nCONTACT LIST\n");
   for (int i = 0; i < 3; i++) {
      PrintContactNode(current);
      printf("\n");
      current = current->nextNodePtr;
   }
   
   free(headNode);
   free(secondNode);
   free(thirdNode);
   
   return 0;
}

