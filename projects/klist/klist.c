#include <stdio.h>
#include <malloc.h>

#include "klist.h"


static int free_cnt;
static int malloc_cnt;

#define free(p) free((++free_cnt, p))
#define malloc(s) malloc((++malloc_cnt, s))


// basic functions
void init(List *pL)
{
	pL->cnt = 0;
	pL->head = pL->tail = NULL;
}

void clear(List *pL)
{
	Node *p, *q;
	for (p=pL->head; p; p=q)
	{
		q = p->next;
		free(p);
	}
	pL->head = pL->tail = NULL;
	pL->cnt = 0;
}

bool empty(const List *pL)
{
	return !(pL->cnt);
}

cint size(const List *pL)
{
	return pL->cnt;
}

List *copy(List *pDest, const List *pSrc)
{
	clear(pDest);
	const Node *p;
	for (p=pSrc->head; p; p=p->next)
		push_back(pDest, p->data);

	return pDest;
}

List *append(List *pDest, const List *pSrc)
{
	const Node *p;
	for (p=pSrc->head; p; p=p->next)
		push_back(pDest, p->data);

	return pDest;
}

List *merge(List *pD, const List *pS1, const List *pS2)
{
	clear(pD);
	const Node *p = pS1->head, *q = pS2->head;
	while (p && q)
	{
		if (less(p->data, q->data))
			push_back(pD, p->data), p = p->next;
		else
			push_back(pD, q->data), q = q->next;
	}

	while (p)
		push_back(pD, p->data), p = p->next;
	while (q)
		push_back(pD, q->data), q = q->next;
	
	return pD;
}



// pushing and popping
static void push_first(List *pL, Data data)
{
	++pL->cnt;
	Node *newNode = (Node *)malloc(sizeof(Node));
	init_node(newNode, data, NULL, NULL);
	pL->head = pL->tail = newNode;
}

bool push_front(List *pL, Data data)
{
	if (pL->cnt >= CAPACITY)
		return false;		// guard clause

	if (pL->cnt)
	{
		++pL->cnt;
		Node *newNode = (Node *)malloc(sizeof(Node));
		init_node(newNode, data, NULL, pL->head);
		pL->head = pL->head->prev = newNode;
	}
	else push_first(pL, data);

	return true;
}

bool push_back(List *pL, Data data)
{
	if (pL->cnt >= CAPACITY)
		return false;		// guard clause

	if (pL->cnt)
	{
		++pL->cnt;
		Node *newNode = (Node *)malloc(sizeof(Node));
		init_node(newNode, data, pL->tail, NULL);
		pL->tail = pL->tail->next = newNode;
	}
	else push_first(pL, data);

	return true;
}

static Data pop_last(List *pL)
{
	Data data = pL->head->data;
	free(pL->head), --pL->cnt;
	pL->head = pL->tail = NULL;
	return data;
}

Data pop_front(List *pL)
{
	if (pL->cnt)
	{
		if (pL->cnt == 1)
			return pop_last(pL);	// guard clause

		Node *oldHead = pL->head;
		pL->head = pL->head->next;
		pL->head->prev = NULL;
		Data data = oldHead->data;
		free(oldHead), --pL->cnt;
		return data;
	}

	return INVALID_DATA;
}

Data pop_back(List *pL)
{
	if (pL->cnt)
	{
		if (pL->cnt == 1)
			return pop_last(pL);	// guard clause

		Node *oldTail = pL->tail;
		pL->tail = pL->tail->prev;
		pL->tail->next = NULL;
		Data data = oldTail->data;
		free(oldTail), --pL->cnt;
		return data;
	}

	return INVALID_DATA;
}



// find
xint find(const List *pL, const Data key)
{
	Node *p;
	bool found = false;
	xint now = ZERO_INDEX;
	for (p=pL->head; p; p=p->next, ++now)
	{
		if (equal(p->data, key))
		{
			found = true;
			break;
		}
	}

	return found ? now : INVALID_INDEX;
}

xint rfind(const List *pL, const Data key)
{
	Node *p;
	bool found = false;
	xint now = pL->cnt-1 + ZERO_INDEX;
	for (p=pL->tail; p; p=p->prev, --now)
	{
		if (equal(p->data, key))
		{
			found = true;
			break;
		}
	}

	return found ? now : INVALID_INDEX;
}

static Node *ptr_at(const List *pL, const xint index)
{
	if (index >= ZERO_INDEX && index <= pL->cnt-1 + ZERO_INDEX)
	{
		Node *p;
		xint now;
		if (index < pL->cnt / 2)
		{
			now = ZERO_INDEX;
			for (p=pL->head; now<index; p=p->next, ++now)
			{
				;
			}
		}
		else
		{
			now = pL->cnt-1 + ZERO_INDEX;
			for (p=pL->tail; now>index; p=p->prev, --now)
			{
				;
			}
		}

		return p;
	}

	return NULL;
}

Data at(const List *pL, const xint index)
{
	Node *p = ptr_at(pL, index);
	if (p)
	{
		return p->data;
	}

	return INVALID_DATA;
}



// insertion and erasion
bool insert_before(List *pL, const xint index, const Data data)
{
	if (pL->cnt >= CAPACITY)
		return false;		// guard clause

	Node *p = ptr_at(pL, index);
	if (p)
	{
		if (p != pL->head)
		{
			++pL->cnt;
			Node *newNode = (Node *)malloc(sizeof(Node));
			init_node(newNode, data, p->prev, p);
			newNode->prev->next = newNode;
			newNode->next->prev = newNode;
		}
		else push_front(pL, data);

		return true;
	}

	return false;
}

bool insert_after(List *pL, const xint index, const Data data)
{
	if (pL->cnt >= CAPACITY)
		return false;		// guard clause

	Node *p = ptr_at(pL, index);
	if (p)
	{
		if (p != pL->tail)
		{
			++pL->cnt;
			Node *newNode = (Node *)malloc(sizeof(Node));
			init_node(newNode, data, p, p->next);
			newNode->prev->next = newNode;
			newNode->next->prev = newNode;
		}
		else push_back(pL, data);

		return true;
	}

	return false;
}

Data erase_node(List *pL, Node *p)
{
	if (p == pL->head)
		return pop_front(pL);
	if (p == pL->tail)
		return pop_back(pL);

	Data data = p->data;
	p->prev->next = p->next;
	p->next->prev = p->prev;
	free(p), --pL->cnt;

	return data;
}

cint erase_tagged(List *pL)
{
	cint erased_cnt = 0;
	Node *p, *q;
	for (p=pL->head; p; p=q)
	{
		q = p->next;
		if (p->tag)
		{
			++erased_cnt;
			erase_node(pL, p);
		}
	}

	return erased_cnt;
}

xint erase_first_key(List *pL, const Data key)
{
	xint now = ZERO_INDEX;
	bool found = false;
	Node *p, *q;
	for (p=pL->head; p; p=q, ++now)
	{
		q = p->next;
		if (equal(p->data, key))
		{
			found = true;
			erase_node(pL, p);
			break;
		}
	}

	return found ? now : INVALID_INDEX;
}

xint erase_last_key(List *pL, const Data key)
{
	xint now = pL->cnt-1 + ZERO_INDEX;
	bool found = false;
	Node *p, *q;
	for (p=pL->tail; p; p=q, --now)
	{
		q = p->prev;
		if (equal(p->data, key))
		{
			found = true;
			erase_node(pL, p);
			break;
		}
	}

	return found ? now : INVALID_INDEX;
}

cint erase_keys(List *pL, const Data key)
{
	cint erased_cnt = 0;
	Node *p, *q;
	for (p=pL->head; p; p=q)
	{
		q = p->next;
		if (equal(p->data, key))
		{
			++erased_cnt;
			erase_node(pL, p);
		}
	}

	return erased_cnt;
}

Data erase_at(List *pL, const xint index)
{
	Node *p = ptr_at(pL, index);
	if (p)
	{
		return erase_node(pL, p);
	}

	return INVALID_DATA;
}

cint erase_interval(List *pL, const xint lb, const xint ub)	// [lb, ub)
{
	if (lb >= ub)
		return 0;		// guard clause

	cint erased_cnt = 0;
	Node *pLB = ptr_at(pL, lb);
	Node *pUB = ptr_at(pL, ub-1);

	if (pLB && pUB)
	{
		Node *p, *q, *end = pUB->next;	// Restore the pUB->next. Because *pUB will be erased, too.
		for (p=pLB; p!=end; p=q)
		{
			q = p->next;
			++erased_cnt;
			erase_node(pL, p);
		}
	}

	return erased_cnt;	// obviously, erased_cnt == (ub-lb)
}



// other operations to the data
Data add_at(List *pL, const xint index, const Data data)
{
	Node *p = ptr_at(pL, index);
	if (p)
	{
		Data oldData = p->data;
		p->data += data;
		return oldData;
	}

	return INVALID_DATA;
}

cint add_interval(List *pL, const xint lb, const xint ub, const Data data)
{
	if (lb >= ub)
		return 0;		// guard clause

	cint added_cnt = 0;
	Node *pLB = ptr_at(pL, lb);
	Node *pUB = ptr_at(pL, ub-1);

	if (pLB && pUB)
	{
		Node *p;
		for (p=pLB; p!=pUB->next; p=p->next)
		{
			++added_cnt;
			p->data += data;
		}
	}

	return added_cnt;	// obviously, added_cnt == (ub-lb)
}

Data assign_at(List *pL, const xint index, const Data data)
{
	Node *p = ptr_at(pL, index);
	if (p)
	{
		Data oldData = p->data;
		p->data = data;
		return oldData;
	}

	return INVALID_DATA;
}

cint assign_interval(List *pL, const xint lb, const xint ub, const Data data)
{
	if (lb >= ub)
		return 0;		// guard clause

	cint assigned_cnt = 0;
	Node *pLB = ptr_at(pL, lb);
	Node *pUB = ptr_at(pL, ub-1);

	if (pLB && pUB)
	{
		Node *p;
		for (p=pLB; p!=pUB->next; p=p->next)
		{
			++assigned_cnt;
			p->data = data;
		}
	}

	return assigned_cnt;	// obviously, assigned_cnt == (ub-lb)
}

cint replace(List *pL, const Data oldData, const Data newData)
{
	cint replaced_cnt = 0;
	Node *p;
	for (p=pL->head; p; p=p->next)
	{
		if (equal(p->data, oldData))
		{
			++replaced_cnt;
			p->data = newData;
		}
	}

	return replaced_cnt;
}

bool swap_at(List *pL, const xint left, const xint right)
{
	if (left == right)
		return false;		// guard clause

	Node *p = ptr_at(pL, left);
	Node *q = ptr_at(pL, right);

	if (p && q)
	{
		swap(Data, p->data, q->data);
		return true;
	}

	return false;
}



// other operations to the whole list
void reverse(List *pL)
{
	if (pL->cnt)
	{
		Node *p, *q;
		for (p=pL->head; p; p=q)
		{
			q = p->next;
			swap(Node *, p->prev, p->next);
		}
		swap(Node *, pL->head, pL->tail);
	}
}

void unique(List *pL)
{
	if (pL->cnt < 2)
		return;		// guard clause

	Node *p, *q;
	for (p=pL->head->next; p; p=q)
	{
		q = p->next;
		if (equal(p->data, p->prev->data))
		{
			erase_node(pL, p);
		}
	}
}

void bubble_sort(List *pL)
{
	if (pL->cnt < 2)
		return;		// guard clause

	Node *end, *p;
	for (end=pL->tail; end!=pL->head; end=end->prev)
	{
		for (p=pL->head; p!=end; p=p->next)
		{
			if (greater(p->data, p->next->data))
			{
				swap(Data, p->data, p->next->data);
			}
		}
	}
}

void println(const List *pL)
{
	const Node *p;
	for (p=pL->head; p; p=p->next)
		printf("%d ", p->data);
	putchar('\n');
}



// debug function (print the the number of times that the free and malloc functions are called)
void debug_allocate()
{
	printf("malloc : %d times.\n", malloc_cnt);
	printf("free   : %d times.\n", free_cnt);
}
