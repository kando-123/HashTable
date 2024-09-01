#pragma warning(disable : 4996)

#include <stdio.h>
#include "HashTable.h"

void PrintHashTable(HashTable_S* table)
{
	puts("Hash Table");
	if (!table)
	{
		puts("\t(null)");
	}
	else
	{
		printf("\tsize = %llu\n", table->size);
		printf("\tcapacity = %llu\n", table->capacity);
		puts("\tbuckets:");
		for (size_t i = 0; i < table->capacity; ++i)
		{
			printf("\t\tb. #%llu: ", i);
			for (Node_S* node = table->buckets[i].head; node; node = node->next)
			{
				printf("[%s:%i] -> ", node->key, node->value);
			}
			puts("(null)");
		}
	}
}

void PrintMenu(void)
{
	puts("insert: i <key:string> <value:integer>");
	puts("search: s <key:string>");
	puts("remove: r <key:string>");
	puts("print:  p");
	puts("quit:   q");
	puts("menu:   m");
}

int main()
{
	HashTable_S hashTable;
	HashTable_Init(&hashTable);

	char key[100] = { 0 };
	int value = 0;

	PrintMenu();
	for (char ch = getchar(); ch != 'q' && ch != 'Q'; ch = getchar())
	{
		switch (ch)
		{
		case 'I':
		case 'i':
			if (scanf("%s %i", key, &value) == 2)
			{
				printf("Inserting: (%s, %i)\n", key, value);
				HashTable_Insert(&hashTable, key, value);
			}
			else
			{
				puts("Input error. Continue.");
			}
			break;
		case 'S':
		case 's':
			if (scanf("%s", key) == 1)
			{
				printf("Searching for: %s\n", key);
				if (HashTable_Search(&hashTable, key, &value) == HASHTABLE_SUCCESS)
				{
					printf("Value: %i\n", value);
				}
				else
				{
					puts("Mapping not found.");
				}
			}
			else
			{
				puts("Input error. Continue.");
			}
			break;
		case 'R':
		case 'r':
			if (scanf("%s", key) == 1)
			{
				printf("Removing: %s\n", key);
				HashTable_Remove(&hashTable, key);
			}
			else
			{
				puts("Input error. Continue.");
			}
			break;
		case 'P':
		case 'p':
			PrintHashTable(&hashTable);
			break;
		case 'M':
		case 'm':
			PrintMenu();
			break;
		case 'C':
		case 'c':
			HashTable_DeInit(&hashTable);
			HashTable_Init(&hashTable);
			break;
		}
	}

	HashTable_DeInit(&hashTable);

	return 0;
}