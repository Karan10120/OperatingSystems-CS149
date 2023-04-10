#include<stdio.h>
#include<string.h>

#include "ItemToPurchase.h"

int main(void) {
   char input [64];
   ItemToPurchase item1;
   ItemToPurchase item2;

   MakeItemBlank(&item1);
   MakeItemBlank(&item2);

   printf("Item 1\n");
   printf("Enter the item name:\n");
   fgets(input, 64, stdin);
   input[strcspn(input, "\n")] = 0;
   strcpy(item1.itemName, input);
   printf("Enter the item price:\n");
   scanf("%d\n", &item1.itemPrice);
   printf("Enter the item quantity:\n");
   scanf("%d", &item1.itemQuantity);

   char c = getchar();
   while(c != '\n' && c != EOF) {
      c = getchar();
   }

   printf("\nItem 2\n");
   printf("Enter the item name:\n");
   fgets(input, 64, stdin);
   input[strcspn(input, "\n")] = 0;
   strcpy(item2.itemName, input);
   printf("Enter the item price:\n");
   scanf("%d\n", &item2.itemPrice);
   printf("Enter the item quantity:\n");
   scanf("%d\n", &item2.itemQuantity);

   printf("\nTOTAL COST\n");
   PrintItemCost(item1);
   PrintItemCost(item2);
   printf("\nTotal: $%d\n", ((item1.itemPrice * item1.itemQuantity)+(item2.itemPrice * item2.itemQuantity)));

   return 0;
}
