#pragma once

typedef char* String_T;

#define HASHTABLE_SUCCESS 0
#define HASHTABLE_FAILURE 1

/* ---------- Node ---------- */

struct Node
{
	String_T key;
	int value;

	struct Node* next;
};

typedef struct Node Node_S;

/* ---------- Bucket ---------- */

struct Bucket
{
	Node_S* head;
};

typedef struct Bucket Bucket_S;

/* ---------- Table ---------- */

struct HashTable
{
	size_t size;
	size_t capacity;

	Bucket_S* buckets;

};

typedef struct HashTable HashTable_S;

int HashTable_Init(HashTable_S* table);
int HashTable_DeInit(HashTable_S* table);

int HashTable_Insert(HashTable_S* table, const String_T key, int value);
int HashTable_Search(HashTable_S* table, const String_T key, int* value);
int HashTable_Remove(HashTable_S* table, const String_T key);
