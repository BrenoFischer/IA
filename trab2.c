#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define INSTANCES 958
#define K 3
#define TREINAMENTO 767

typedef struct
{
	int pos;
	int neg;
} Conjunto;

typedef struct
{
	float d;
	char class[9];
} Dist;

typedef struct
{
	char squares[9];
	/*
	char tl;
	char tm;
	char tr;
	char ml;
	char mm;
	char mr;
	char bl;
	char bm;
	char br;
*/
	char class[9];
} Item;

/* Se for um nó folha, terá uma classe correspondente (0: negativo, 1:positivo), caso contrário terá -1 */
typedef struct no
{
	Conjunto s;
	Item *itens;
	int atributo;
	int classe; //0 negativo 1 positivo
	struct no *noX;
	struct no *noO;
	struct no *noB;
} No;

Item m[INSTANCES];

int ATRIBUTOS[9];

void le_itens(FILE *p)
{
	int i = 0, k = 0, item = 0;
	char ch;

	do
	{
		for (i = 0, k = 0; i < 18; i++)
		{
			ch = fgetc(p);
			if (ch != ',')
			{
				m[item].squares[k] = ch;
				k++;
			}
		}
		k = 0;

		fgets(m[item].class, sizeof(m[item].class), p);

		ch = fgetc(p);
		item++;
	} while (ch != EOF);
}

void mostra_n_itens(int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%d: ", i);
		for (int j = 0; j < 9; j++)
		{
			printf("%c,", m[i].squares[j]);
		}
		printf("%s\n", m[i].class);
	}
}

float dist_euclidiana(Item i1, Item i2)
{
	int sum = 0;
	for (int i = 0; i < 9; i++)
	{
		if (i1.squares[i] != i2.squares[i])
		{
			sum += 1;
		}
	}
	return sqrt(sum);
}

void embaralha_n_itens(int n)
{
	int r1, r2, i, j;
	Item aux;

	srand(time(NULL));
	r1 = rand() % INSTANCES;
	r2 = rand() % INSTANCES;
	while (r2 == r1)
	{
		r2 = rand() % INSTANCES;
	}

	for (i = 0; i < n; i++)
	{
		for (j = 0; j < 9; j++)
		{
			aux.squares[j] = m[r1].squares[j];
			m[r1].squares[j] = m[r2].squares[j];
			m[r2].squares[j] = aux.squares[j];
		}
		strcpy(aux.class, m[r1].class);
		strcpy(m[r1].class, m[r2].class);
		strcpy(m[r2].class, aux.class);

		r1 = rand() % INSTANCES;
		r2 = rand() % INSTANCES;
		while (r2 == r1)
		{
			r2 = rand() % INSTANCES;
		}
	}
}

void insere(Item *itens, int n, int max)
{
	for (int i = n, z = 0; i < max; i++, z++)
	{
		for (int j = 0; j < 9; j++)
		{
			itens[z].squares[j] = m[i].squares[j];
		}
		strcpy(itens[z].class, m[i].class);
	}
}

void insere_treinamento_classificacao(Item *treinamento, Item *classificacao)
{
	insere(treinamento, 0, TREINAMENTO);
	insere(classificacao, TREINAMENTO, INSTANCES);
}
/*
void verifica_distancias(Item item, Item *treinamento, Dist *distancias)
{
	float aux;
	for (int i = 0; i < K; i++)
	{
		distancias[i].d = 99.9;
	}
	for (int i = 0; i < TREINAMENTO; i++)
	{
		aux = dist_euclidiana(item, treinamento[i]);
		for (int j = 0; j < K; j++)
		{
			if (aux < distancias[j].d)
			{
				distancias[j].d = aux;
				strcpy(distancias[j].class, treinamento[i].class);
			}
		}
	}
}

void classifica(Item *itens, Item *treinamento)
{
	int n = INSTANCES - TREINAMENTO;
	Dist *vizinhos;
	int count = 0, countP = 0, countN = 0;
	char aux[9];

	for (int i = 0; i < n; i++)
	{
		verifica_distancias(itens[i], treinamento, vizinhos);
		for (int j = 0; j < K; j++)
		{
			printf("%d  %0.2f  %s\n", j, vizinhos[j].d, vizinhos[j].class);
			// if (vizinhos[j].class[0] == 'p')
			// 	countP++;
			// else
			// 	countN++;
		}
		printf("%s\n", itens[i].class);
		// if (countP > countN)
		// 	strcpy(aux, "positive");
		// else
		// 	strcpy(aux, "negative");

		// if (aux[0] != itens[i].class[0])
		// 	count++;
		// countP = 0;
		// countN = 0;
	}
}

void knn()
{
	Item treinamento[TREINAMENTO];
	Item classificacao[INSTANCES - TREINAMENTO];

	embaralha_n_itens(1000);
	insere_treinamento_classificacao(treinamento, classificacao);

	classifica(classificacao, treinamento);
}

*/
/* ID3 */
float entropia(Conjunto s)
{
	float total = s.pos + s.neg;
	float pMais = s.pos / total;
	float pMenos = s.neg / total;
	return (-(pMais * log(pMais) / log(2)) - (pMenos * log(pMenos) / log(2)));
}

void inicializa_conjunto(Conjunto *s)
{
	s->pos = 0;
	s->neg = 0;
}

void inicializa_conjuntos(Conjunto *s1, Conjunto *s2, Conjunto *s3)
{
	inicializa_conjunto(s1);
	inicializa_conjunto(s2);
	inicializa_conjunto(s3);
}

void verifica_classe(Item *item, Conjunto *s)
{
	if (item->class[0] == 'p')
		s->pos++;
	else
		s->neg++;
}

Conjunto cria_conjunto_s(Item *itens, int n)
{
	Conjunto s;
	inicializa_conjunto(&s);

	for (int i = 0; i < n; i++)
		verifica_classe(&itens[i], &s);
	return s;
}

int verifica_positivos_conjunto(Conjunto s)
{
	return (s.neg == 0);
}

int verifica_negativos_conjunto(Conjunto s)
{
	return (s.pos == 0);
}

float ganho_de_informacao(Conjunto s1, Conjunto s2, Conjunto s3, Conjunto s)
{
	float entropiaS = entropia(s);
	float totalS = s.pos + s.neg;
	float totalS1 = (s1.pos + s1.neg) / totalS, totalS2 = (s2.neg + s2.pos) / totalS, totalS3 = (s3.neg + s3.pos) / totalS;

	return entropiaS - (totalS1 * entropia(s1)) - (totalS2 * entropia(s2)) - (totalS3 * entropia(s3));
}

//atributo: 0 - 8 squares
float avalia_atributo(int atributo, int n, Item *itens, Conjunto novoX, Conjunto novoO, Conjunto novoB, Conjunto s)
{
	for (int i = 0; i < n; i++)
	{
		if (itens[i].squares[atributo] == 'x')
			verifica_classe(&itens[i], &novoX);

		else if (itens[i].squares[atributo] == 'o')
			verifica_classe(&itens[i], &novoO);

		else
			verifica_classe(&itens[i], &novoB);
	}
	printf("X:[+%d, -%d]  O:[+%d, -%d], B:[+%d, -%d]\n", novoX.pos, novoX.neg, novoO.pos, novoO.neg, novoB.pos, novoB.neg);
	return ganho_de_informacao(novoX, novoO, novoB, s);
}

void inicializa_atributos()
{
	for (int i = 0; i < 9; i++)
	{
		ATRIBUTOS[i] = 0;
	}
}

void retira_atributo(int i)
{
	ATRIBUTOS[i] = -1;
}

void insere_informacoes_no(No *no, int atributo, Conjunto s, Item *itens, No *no_x, No *no_o, No *no_b, int classe)
{
	no->atributo = atributo;
	no->classe = classe;
	no->s = s;
	no->itens = itens;
	no->noB = no_b;
	no->noO = no_o;
	no->noX = no_x;
}

No *cria_no()
{
	No *p = (No *)malloc(sizeof(No));
	if (p == NULL)
		exit(1);
	p->atributo = -1;
	p->itens = NULL;
	p->noB = NULL;
	p->noO = NULL;
	p->noX = NULL;
	return p;
}

No *monta_arvore_aux(Item *itens, int n)
{
	Item *itensX, *itensO, *itensB;
	float *ganhos, maiorGanho = 0.0f;
	int atributoGanho, countX = 0, countO = 0, countB = 0;
	int quantidade_atributos = 9;
	No *novo = cria_no(), *no_x = cria_no(), *no_o = cria_no(), *no_b = cria_no();
	Conjunto s = cria_conjunto_s(itens, n);
	Conjunto novoX, novoO, novoB;
	float entropiaS = entropia(s);
	printf("Entropia do conjunto do nó: %f\n", entropiaS);

	if (verifica_positivos_conjunto(s))
	{
		insere_informacoes_no(novo, -1, s, NULL, NULL, NULL, NULL, 1);
		return novo;
	}
	else if (verifica_negativos_conjunto(s))
	{
		insere_informacoes_no(novo, -1, s, NULL, NULL, NULL, NULL, 0);
		return novo;
	}

	else
	{
		inicializa_conjuntos(&novoX, &novoO, &novoB);
		for (int i = 0; i < 9; i++)
		{
			if (ATRIBUTOS[i] == -1)
				quantidade_atributos--;
		}
		ganhos = (float *)malloc(quantidade_atributos * sizeof(float));
		for (int i = 0; i < quantidade_atributos; i++)
		{
			if (ATRIBUTOS[i] != -1)
			{
				ganhos[i] = avalia_atributo(i, n, itens, novoX, novoO, novoB, s);
				printf("ganhos[%d]: %f\n", i, ganhos[i]);
				printf("maior ganho: %f\n", maiorGanho);
				if (ganhos[i] > maiorGanho)
				{
					maiorGanho = ganhos[i];
					atributoGanho = i;
				}
			}
		}
		printf("Maior ganho: %f   atributoGanho: %d\n", maiorGanho, atributoGanho);
		retira_atributo(atributoGanho);
		for (int i = 0; i < 9; i++)
			printf("ATRIBUTOS[%d]: %d\n", i, ATRIBUTOS[i]);

		insere_informacoes_no(novo, atributoGanho, s, itens, NULL, NULL, NULL, -1);

		for (int i = 0; i < n; i++)
		{
			if (itens[i].squares[atributoGanho] == 'x')
				countX++;
			else if (itens[i].squares[atributoGanho] == 'o')
				countO++;
			else
				countB++;
		}
		if (countX == 0)
		{
			printf("Sem itens\n");
			return novo;
		}
		else
		{
			itensX = (Item *)malloc(countX * sizeof(Item));
			itensO = (Item *)malloc(countO * sizeof(Item));
			itensB = (Item *)malloc(countB * sizeof(Item));

			for (int i = 0, j = 0; i < n; i++)
			{
				if (itens[i].squares[atributoGanho] == 'x')
				{
					for (int k = 0; k < 9; k++)
					{
						itensX[j].squares[k] = itens[i].squares[k];
					}
					strcpy(itensX[j].class, itens[i].class);
					j++;
				}
			}
			printf("Itens com x: %d com o: %d com b: %d -> soma: %d\n", countX, countO, countB, countX + countO + countB);
			//no_x = monta_arvore_aux(itensX, countX);
			//novo->noX = no_x;
			return novo;
		}
	}
}

No *monta_arvore()
{
	Item treinamento[TREINAMENTO];
	Item classificacao[INSTANCES - TREINAMENTO];
	Item *itensX, *itensO, *itensB;

	embaralha_n_itens(1000);
	insere_treinamento_classificacao(treinamento, classificacao);

	Conjunto sInicial = cria_conjunto_s(treinamento, TREINAMENTO);
	Conjunto novoX, novoO, novoB;
	No *no_inicial = cria_no(), *no_x = cria_no(), *no_o = cria_no(), *no_b = cria_no();
	float ganhos[9], maiorGanho = 0.0f;
	int atributoGanho, countX = 0, countO = 0, countB = 0;
	float entropiaSInicial = entropia(sInicial);
	printf("Entropia inicial: %0.4f\n", entropiaSInicial);

	inicializa_atributos();
	inicializa_conjuntos(&novoX, &novoO, &novoB);

	for (int i = 0; i < 9; i++)
	{
		ganhos[i] = avalia_atributo(i, TREINAMENTO, treinamento, novoX, novoO, novoB, sInicial);
		printf("Ganho do atributo %d: %0.4f\n", i + 1, ganhos[i]);
		if (ganhos[i] > maiorGanho)
		{
			maiorGanho = ganhos[i];
			atributoGanho = i;
		}
	}
	printf("Maior ganho: %0.4f, atributo %d\n", maiorGanho, atributoGanho);
	retira_atributo(atributoGanho);

	insere_informacoes_no(no_inicial, atributoGanho, sInicial, treinamento, no_x, no_o, no_b, -1);

	for (int i = 0; i < TREINAMENTO; i++)
	{
		if (treinamento[i].squares[atributoGanho] == 'x')
			countX++;
		else if (treinamento[i].squares[atributoGanho] == 'o')
			countO++;
		else
			countB++;
	}
	itensX = (Item *)malloc(countX * sizeof(Item));
	itensO = (Item *)malloc(countO * sizeof(Item));
	itensB = (Item *)malloc(countB * sizeof(Item));

	for (int i = 0, j = 0; i < TREINAMENTO; i++)
	{
		if (treinamento[i].squares[atributoGanho] == 'x')
		{
			for (int k = 0; k < 9; k++)
			{
				itensX[j].squares[k] = treinamento[i].squares[k];
			}
			strcpy(itensX[j].class, treinamento[i].class);
			j++;
		}
		else if (treinamento[i].squares[atributoGanho] == 'o')
		{
			for (int k = 0; k < 9; k++)
			{
				itensO[j].squares[k] = treinamento[i].squares[k];
			}
			strcpy(itensO[j].class, treinamento[i].class);
			j++;
		}
		else
		{
			for (int k = 0; k < 9; k++)
			{
				itensB[j].squares[k] = treinamento[i].squares[k];
			}
			strcpy(itensB[j].class, treinamento[i].class);
			j++;
		}
	}
	printf("Itens com x: %d com o: %d com b: %d -> soma: %d\n", countX, countO, countB, countX + countO + countB);
	no_x = monta_arvore_aux(itensX, countX);
	no_o = monta_arvore_aux(itensO, countO);
	no_b = monta_arvore_aux(itensB, countB);

	no_inicial->noX = no_x;
	no_inicial->noO = no_o;
	no_inicial->noB = no_b;

	return no_inicial;
}

void id3()
{
	No *arvore;
	arvore = monta_arvore();
	printf("%d\n", arvore->noX->atributo);
	printf("%d\n", arvore->noO->atributo);
	printf("%d\n", arvore->noB->atributo);
}
/* Fim ID3 */

int main(void)
{
	FILE *p;

	p = fopen("tic-tac-toe.data", "r");
	if (p == NULL)
	{
		perror("Erro ao tentar abrir o arquivo.\n");
		exit(EXIT_FAILURE);
	}

	le_itens(p);

	//mostra_n_itens();

	/*
	for(int i = 1; i < INSTANCES - 1; i++) {
		printf("%0.2f\n",dist_euclidiana(m[0], m[i]));
	}
*/

	//mostra_n_itens(INSTANCES);
	// knn();

	id3();

	fclose(p);
	return 0;
}
