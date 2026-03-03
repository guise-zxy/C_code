#define _CRT_SECURE_NO_WARNINGS
#include "heap.h"


void HPInit(HP* php)
{
	assert(php);
	php->a = NULL;
	php->capacity = php->size = 0;
	return;
}



//Ïú»Ù
void HPDestroy(HP* php)
{
	assert(php);
	free(php->a);
	php->a = NULL;
	php->size = php->capacity = 0;

}



/*
void HPPush(HPDataType * a)
{
	assert(a);
	if()

}

*/