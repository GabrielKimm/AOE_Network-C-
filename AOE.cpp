#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


typedef struct Vertex {
	int		count;

	struct Edge *link;
}Vertex;

typedef struct Edge {
	int		vertex;
	int		desti;
	int		duration;
	int		slack;

	struct Edge	*link;
}Edge;


struct Vertex *flist;
struct Vertex *blist;
struct Edge *EdgePtr;


void	read();
void	topSortInit();

void	check_critical();
void	result();
void	clean_memory(int l);

void	mkfront(Vertex* flist, int countValue, int vertexValue, int durationValue);
void	mkback(Vertex* blist, int countValue, int vertexValue, int durationValue);

void	EarlytopSort(Vertex * flist, int VertexCount, int *earliest);
void	LatetopSort(Vertex * blist, int VertexCount, int *latest);

void	Earlyprocess(Vertex *flist, int *early, int *earliest, int VertexCount, int EdgeCount);
void	Lateprocess(Vertex *blist, int *late, int *latest, int VertexCount, int EdgeCount);

static int stack[100];
static int top = -1;
static int dp_event;
static int bigboy = 0;

int		VertexCount;
int		EdgeCount;
int		*earliest;
int		*latest;
int		*early;
int		*late;

void	push(int store) { stack[++top] = store; }
int		pop() { return stack[top--]; }


int		main(int argc, char **argv)
{
	read();
	topSortInit();

	EarlytopSort(flist, VertexCount, earliest);
	LatetopSort(blist, VertexCount, latest);

	Earlyprocess(flist, early, earliest, VertexCount, EdgeCount);
	Lateprocess(blist, late, latest, VertexCount, EdgeCount);

	check_critical();
	result();

	clean_memory(1);
}


void	read()
{

	int countValue, vertexValue, durationValue;

	char fname[99];
	printf("프로그램경로의 불러올 input data 파일을 *.txt 형식으로 입력하시오: ");
	scanf("%s", &fname);

	FILE *fptr = fopen(fname, "r");
	fscanf(fptr, "%d", &VertexCount);
	fscanf(fptr, "%d", &EdgeCount);

	EdgePtr = (Edge *)calloc(sizeof(Edge), (EdgeCount));

	flist = (Vertex *)calloc(sizeof(Vertex), (VertexCount));
	blist = (Vertex *)calloc(sizeof(Vertex), (VertexCount));


	for (int i = 0; i < EdgeCount; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (j == 0) { fscanf(fptr, "%d", &countValue); }
			else if (j == 1) { fscanf(fptr, "%d", &vertexValue); }
			else if (j == 2) { fscanf(fptr, "%d", &durationValue); }
			else
			{
				printf("error");
				system("pause");
				clean_memory(0);
				exit(1);
			}
		}

		EdgePtr[i].vertex = countValue;
		EdgePtr[i].desti = vertexValue;
		EdgePtr[i].duration = durationValue;

		flist[vertexValue].count++;
		blist[countValue].count++;

		mkfront(flist, countValue, vertexValue, durationValue);
		mkback(blist, countValue, vertexValue, durationValue);
	}
}


void	mkfront(Vertex* flist, int countValue, int vertexValue, int durationValue)
{
	struct Edge *new_node;

	new_node = (struct Edge*)malloc(sizeof(struct Edge));
	new_node->vertex = vertexValue;
	new_node->duration = durationValue;
	new_node->link = NULL;

	if (!flist[countValue].link)
	{
		flist[countValue].link = new_node;
	}
	else
	{
		struct Edge *cur;
		for (cur = flist[countValue].link; cur->link != NULL; cur = cur->link) {}
		cur->link = new_node;
	}
}


void	mkback(Vertex* blist, int countValue, int vertexValue, int durationValue)
{
	struct Edge *new_node;

	new_node = (struct Edge*)malloc(sizeof(struct Edge));
	new_node->vertex = countValue;
	new_node->duration = durationValue;
	new_node->link = NULL;

	if (!blist[vertexValue].link)
	{
		blist[vertexValue].link = new_node;
	}
	else
	{
		struct Edge *cur;
		for (cur = blist[vertexValue].link; cur->link != NULL; cur = cur->link) {}
		cur->link = new_node;
	}
}


void	 topSortInit()
{
	earliest = (int*)calloc(sizeof(int), (VertexCount));
	latest = (int*)calloc(sizeof(int), (VertexCount));
	early = (int*)calloc(sizeof(int), (EdgeCount));
	late = (int*)calloc(sizeof(int), (EdgeCount));
}


void	EarlytopSort(Vertex * flist, int VertexCount, int *earliest)
{
	struct Edge *cur;
	int k;

	for (int i = 0; i < VertexCount; i++)
	{
		if (!flist[i].count)
		{
			push(i);
		}
	}

	for (int i = 0; i < VertexCount; i++)
	{
		if (top == -1)
		{
			printf("network has a cycle\n");
			system("pause");
			clean_memory(1);
			exit(1);
		}
		else
		{
			int j = pop();
			dp_event = j;

			for (cur = flist[j].link; cur != NULL; cur = cur->link)
			{
				k = cur->vertex;
				flist[k].count--;
				if (!flist[k].count)
				{
					push(k);
				}
				if (earliest[k] < earliest[j] + cur->duration)
				{
					earliest[k] = earliest[j] + cur->duration;
					if (bigboy < earliest[k])
					{
						bigboy = earliest[k];
					}
				}
			}
		}
	}
	for (int i = 0; i < VertexCount; i++)
	{
		latest[i] = bigboy;
	}
}


void	LatetopSort(Vertex * blist, int VertexCount, int *latest)
{
	struct Edge *cur;
	int k;

	for (int i = 0; i < VertexCount; i++)
	{
		if (!blist[i].count)
		{
			push(i);
		}
	}

	for (int i = 0; i < VertexCount; i++)
	{
		if (top == -1)
		{
			printf("network has a cycle\n");
			system("pause");
			clean_memory(1);
			exit(1);
		}
		else
		{
			int j = pop();

			for (cur = blist[j].link; cur != NULL; cur = cur->link)
			{
				k = cur->vertex;
				blist[k].count--;
				if (!blist[k].count)
				{
					push(k);
				}
				if (latest[k] > latest[j] - cur->duration)
				{
					latest[k] = latest[j] - cur->duration;
				}
			}
		}
	}

}


void	Earlyprocess(Vertex *flist, int *early, int *earliest, int VertexCount, int EdgeCount)
{
	struct Edge *cur;

	int k = 0;
	for (int i = 0; i < VertexCount; i++)
	{
		early[k++] = earliest[i];

		for (cur = flist[i].link; cur->link != NULL; cur = cur->link)
		{
			early[k++] = earliest[i];
		}

		if (k == EdgeCount) { break; }
	}
}


void	Lateprocess(Vertex *blist, int *late, int *latest, int VertexCount, int EdgeCount)
{
	struct Edge *cur;

	int i = 0;
	for (int l = 0; l < VertexCount; l++)
	{
		cur = blist[l].link;

		for (cur; cur != NULL; cur = cur->link)
		{
			late[i++] = latest[l] - (cur->duration);
		}
	}
}


void	check_critical()
{
	for (int i = 0; i < EdgeCount; i++)
	{
		EdgePtr[i].slack = late[i] - early[i];
	}
}


void	result()
{
	printf("\nCritical path: \n");
	for (int i = 0; i < EdgeCount; i++)
	{
		if (EdgePtr[i].slack == 0)
		{
			printf("%d %d %d \n", EdgePtr[i].vertex, EdgePtr[i].desti, EdgePtr[i].duration);
		}
	}

	printf("\n마지막 이벤트(V%d)이 발생하는 시점 = %d \n\n", dp_event, bigboy);
	system("pause");
}


void	clean_memory(int l)
{
	if (l == 0)
	{
		free(EdgePtr);
		free(flist);
		free(blist);
	}
	else if (l == 1)
	{
		free(EdgePtr);
		free(flist);
		free(blist);
		free(earliest);
		free(latest);
		free(early);
		free(late);
	}
	else
	{
		printf("error");
		system("pause");
		exit(1);
	}
}