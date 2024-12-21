#include "HashTable.h"

#include <stdlib.h>
#include <string.h>

#define NODE_SUCCESS 0
#define NODE_FAILURE 1

/* ---------- Node ---------- */

int Node_Init(Node_S* node, const String_T key, int value)
{
	if (node)
	{
		size_t length = strlen(key) + 1;
		String_T copy = malloc(length);
		if (copy)
		{
			memcpy(copy, key, length);
			node->key = copy;
			node->value = value;
			node->next = NULL;
			return NODE_SUCCESS;
		}
	}
	return NODE_FAILURE;
}

void Node_DeInit(Node_S* node)
{
	if (node)
	{
		Node_DeInit(node->next);
		free(node->next);
		free(node->key);
	}
}

/* ---------- Bucket ---------- */

void Bucket_Clear(Bucket_S* bucket)
{
	Node_DeInit(bucket->head);
}

/* ---------- Table ---------- */

#define INITIAL_CAPACITY 8

int HashTable_Init(HashTable_S* table)
{
	if (!table)
	{
		return HASHTABLE_FAILURE;
	}

	table->size = 0;
	table->capacity = INITIAL_CAPACITY;
	table->buckets = calloc(table->capacity, sizeof(Bucket_S));

	return table->buckets ? HASHTABLE_SUCCESS : HASHTABLE_FAILURE;
}

int HashTable_DeInit(HashTable_S* table)
{
	if (!table)
	{
		return HASHTABLE_FAILURE;
	}

	for (size_t i = 0; i < table->capacity; ++i)
	{
		Bucket_Clear(&table->buckets[i]);
	}
	free(table->buckets);
	table->buckets = NULL;
	table->capacity = 0;
	table->size = 0;

	return HASHTABLE_SUCCESS;
}

#define FACTOR 3

size_t Hash(String_T key)
{
	size_t sum = 0;
	for (size_t factor = 1; *key; ++key)
	{
		sum += (size_t)(*key) * (factor *= FACTOR);
	}
	return sum;
}

int Rehash(HashTable_S* table)
{
	/* Prepare the array for the new buckets. */
	size_t newCapacity = table->capacity << 1;
	Bucket_S* newBuckets = calloc(newCapacity, sizeof(Bucket_S));
	if (!newBuckets)
	{
		return HASHTABLE_FAILURE;
	}

	/* Transfer the nodes by re-pinning them. */
	for (size_t i = 0; i < table->capacity; ++i)
	{
		Node_S* head = table->buckets[i].head;
		while (head)
		{
			/* Extract the first node in the list. */
			Node_S* node = head;
			head = head->next;

			/* Re-pin the node. */
			size_t newIndex = Hash(node->key) & (newCapacity - 1);
			node->next = newBuckets[newIndex].head;
			newBuckets[newIndex].head = node;
		}
	}

	/* Release the previous array of pointers. */
	free(table->buckets);
	table->buckets = newBuckets;
	table->capacity = newCapacity;

	return HASHTABLE_SUCCESS;
}

int Dehash(HashTable_S* table)
{
	/* Prepare the array for the new buckets. */
	size_t newCapacity = table->capacity >> 1;
	Bucket_S* newBuckets = calloc(newCapacity, sizeof(Bucket_S));
	if (!newBuckets)
	{
		return HASHTABLE_FAILURE;
	}

	/* Transfer the nodes by re-pinning them. */
	for (size_t i = 0; i < table->capacity; ++i)
	{
		Node_S* head = table->buckets[i].head;
		while (head)
		{
			/* Extract the first node in the list. */
			Node_S* node = head;
			head = head->next;

			/* Re-pin the node. */
			size_t newIndex = Hash(node->key) & (newCapacity - 1);
			node->next = newBuckets[newIndex].head;
			newBuckets[newIndex].head = node;
		}
	}

	/* Release the previous array of pointers. */
	free(table->buckets);
	table->buckets = newBuckets;
	table->capacity = newCapacity;

	return HASHTABLE_SUCCESS;
}

#define REHASH_FACTOR 2.0
#define DEHASH_FACTOR 0.5

double LoadFactor(HashTable_S* table)
{
	return (double)table->size / (double)table->capacity;
}

int HashTable_Insert(HashTable_S* table, const String_T key, int value)
{
	if (!table || !key)
	{
		return HASHTABLE_FAILURE;
	}

	if (LoadFactor(table) >= REHASH_FACTOR)
	{
		if (Rehash(table) != HASHTABLE_SUCCESS)
		{
			return HASHTABLE_FAILURE;
		}
	}

	size_t index = Hash(key) & (table->capacity - 1);
	if (table->buckets[index].head)
	{
		Node_S* node = table->buckets[index].head;
		Node_S* prev = NULL;
		while (node)
		{
			if (!strcmp(node->key, key))
			{
				node->value = value;
				return HASHTABLE_SUCCESS;
			}
			prev = node;
			node = node->next;
		}
		node = malloc(sizeof(Node_S));
		if (node)
		{
			Node_Init(node, key, value);
			prev->next = node;
			++table->size;
			return HASHTABLE_SUCCESS;
		}
		else
		{
			return HASHTABLE_FAILURE;
		}
	}
	else
	{
		Node_S* node = malloc(sizeof(Node_S));
		if (node)
		{
			Node_Init(node, key, value);
			table->buckets[index].head = node;
			++table->size;
			return HASHTABLE_SUCCESS;
		}
		else
		{
			return HASHTABLE_FAILURE;
		}
	}
}

int HashTable_Search(HashTable_S* table, const String_T key, int* value)
{
	if (!table || !key || !value)
	{
		return HASHTABLE_FAILURE;
	}

	size_t index = Hash(key) & (table->capacity - 1);
	Node_S* node = table->buckets[index].head;
	while (node)
	{
		if (!strcmp(node->key, key))
		{
			*value = node->value;
			return HASHTABLE_SUCCESS;
		}
		node = node->next;
	}
	return HASHTABLE_FAILURE;
}

int HashTable_Remove(HashTable_S* table, const String_T key)
{
	if (!table || !key)
	{
		return HASHTABLE_FAILURE;
	}

	size_t index = Hash(key) & (table->capacity - 1);
	Node_S* node = table->buckets[index].head;
	Node_S* prev = NULL;
	while (node)
	{
		if (!strcmp(node->key, key))
		{
			if (prev)
			{
				prev->next = node->next;
			}
			else
			{
				table->buckets[index].head = node->next;
			}
			node->next = NULL;
			Node_DeInit(node);
			free(node);
			--table->size;
			if (LoadFactor(table) <= DEHASH_FACTOR && table->capacity > INITIAL_CAPACITY)
			{
				Dehash(table);
			}
			return HASHTABLE_SUCCESS;
		}
		prev = node;
		node = node->next;
	}
	return HASHTABLE_FAILURE;
}
