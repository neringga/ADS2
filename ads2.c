#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct product{
	int id;
	float weight;
	float price;
	float pricePerWeight;
	struct product * next;
};

typedef struct product product;

void insertElementAtTheEnd(product **, float , float );
void rewriteFromListToArray (product **, float  *, float *);
void deletingProductsBiggerThanMaxWeigth (product **, int );
int lengthOfList (product *);
float findMaxSetValue(float *, float *, int, float, int **);
float maybeFurtherProductsCanBeAdded(product *, int , float , float ,float , int );
void recursiveFunction (product *, int , float , int *, float *, float *, int *,float *, int );
void deleteElement (product **, int );
int compareProducts(product *, product *);


int main (int argc, char *argv[]) {

	FILE *iname;
	float maxweigth;
	float productPrice;
	float productWeigth;
	product *productList = NULL;
	float maxValue;

	iname = fopen(argv[1], "r");           
	if (iname == NULL)
	{
		printf("File does not exist");
		return 0;
	}
	fscanf(iname, "%f", &maxweigth);
	if (maxweigth <= 0)
	{
		printf("Invalid input");
			return;
	}
	while (!feof(iname))
	{
		fscanf(iname, "%f", &productWeigth);
		if ( (productWeigth <= 0) ) 
		{
			printf("Invalid input");
			return;
		}
		fscanf(iname, "%f", &productPrice);
		if ( (productPrice <= 0) ) 
		{
			printf("Invalid input");
			return;
		}
		insertElementAtTheEnd(&productList, productWeigth, productPrice);
	}
	fclose(iname);

	deletingProductsBiggerThanMaxWeigth(&productList, maxweigth);
	
	if (productList == NULL)
    {
        printf("Set does not exist");
        return;
    }
	
	int listSize = lengthOfList(productList);
	int *maxSet;
	float weights[listSize];
	float prices[listSize];
	rewriteFromListToArray(&productList, prices, weights);

	float maxSetValue = findMaxSetValue(weights,prices, listSize, maxweigth, &maxSet);

	printf("Max set value: %.2f\n", maxSetValue);
    printf("Set contains:\n");
    for (int i = 0; i < listSize; i++) {
        if (maxSet[i] == 1) {
            printf("%.2f %.2f\n",  weights[i], prices[i]); 
        }
    }
    free(maxSet);

	
	return 0;
}

void rewriteFromListToArray (product **productList, float *price, float *weight) {
	int i = 0;
	while (*productList != NULL) {
		price[i] = (*productList)->price;
		weight[i] = (*productList)->weight;
		*productList = (*productList)->next;
		i++;
	}
	free(productList);
}

void insertElementAtTheEnd(product **head, float weight, float price)
{
	product *temp;
	product *p;
	temp=(product *)malloc(sizeof(product));
		temp->weight = weight ;
		temp->price = price ;
		temp->next = NULL;
		if (*head == NULL)
		{
			*head = temp;
		}
		else
		{
			p = *head;
			while (p->next != NULL) {
				p = p->next;
			}
			p->next = temp;
		}
}

int lengthOfList (product *a)                              //returns length of long int
{
	product *temp = a;
	int length = 0;

	while (temp != NULL)
	{
		temp = temp->next;
		length++;
	}
	return length;
} 

void deletingProductsBiggerThanMaxWeigth (product **productList, int weight)
{
	product *temp = *productList;

	while (temp != NULL)
	{
		if (temp->weight > weight)
		{
			deleteElement(productList, temp->weight);
			temp = *productList;
		}
		else temp = temp->next;
	}

}

void deleteElement (product **productList, int element)
{
    struct product* temp = *productList, *prev;

    if (temp != NULL && temp->weight == element)
    {
        *productList = temp->next;
        free(temp);
        return;
    }

    while (temp != NULL && temp->weight != element)
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return;

    prev->next = temp->next;

    free(temp);

}

float findMaxSetValue(float weights[], float prices[],int size,float maxweigth,int **maxSet)
{
	float currentWeigth = 0.0;
	float currentPrice = 0.0;
	float maxSetValue = 0.0;
	product *products = malloc(size * sizeof(product));
	int *currentSet = calloc(size, sizeof(int));
	*maxSet = malloc(size * sizeof(int));
	
	if (!(products && currentSet && *maxSet))  //jei neatlaisvino atminties
	{
		free(products);
		free(currentSet);
		free(*maxSet);
		*maxSet = NULL;
		return 0;
	}
	
	for (int i = 0; i < size; i++)
	{
		products[i].id = i;
		products[i].weight = weights[i];
		products[i].price = prices[i];
		products[i].pricePerWeight = prices[i] / weights[i];
	}
	//isrikiuoju pagal pricePerWeight mazejimo tvarka
	qsort(products, size, sizeof(product), (int (*)(const void *,const void*))compareProducts);
	
	recursiveFunction (products, size, maxweigth, currentSet, &currentWeigth,
						&currentPrice, *maxSet, &maxSetValue, 0);
	
	free(products);
	free(currentSet);
	
	return maxSetValue;
	
}

int compareProducts(product *product1, product *product2)
{
	if (product1->pricePerWeight > product2->pricePerWeight) {
        return -1;
    }
    if (product1->pricePerWeight < product2->pricePerWeight) {
        return 1;
    }
    return 0;
	
}

void recursiveFunction (product *products, int size, float maxweigth, int *currentSet, float *currentWeigth, float *currentPrice, 
						int *maxSet,float *maxSetValue, int level)
{
	if (level == size) {
		*maxSetValue = *currentPrice;
		memcpy(maxSet, currentSet, size * sizeof(int));
		return;
	}
	
	if (*currentWeigth + products[level].weight <= maxweigth)
	{
		*currentWeigth += products[level].weight;
		*currentPrice += products[level].price;
		currentSet[products[level].id] = 1;
		recursiveFunction(products, size, maxweigth, currentSet, currentWeigth,
							currentPrice, maxSet, maxSetValue, level+1);
		*currentWeigth -= products[level].weight;
		*currentPrice -= products[level].price;
		currentSet[products[level].id] = 0;
	}
	
	if (maybeFurtherProductsCanBeAdded (products, size, maxweigth, *currentWeigth, *currentPrice, level + 1) > *maxSetValue ) {
		recursiveFunction(products, size, maxweigth, currentSet, currentWeigth, currentPrice, maxSet, maxSetValue, level+1);
	}
	
}

float maybeFurtherProductsCanBeAdded (product *products, int size, float maxweigth, float currentWeigth, float currentPrice, int level) 
{
	float remainingCapacity = maxweigth - currentWeigth;
	float priceWithFurtherProductsAdded = currentPrice;
	int level1 = level;
	
	while (level1 < size && products[level1].weight <= remainingCapacity)
	{
		remainingCapacity -= products[level1].weight;
		priceWithFurtherProductsAdded += products[level1].price;
		level1++;
	}
	
	if (level1 < level)       //tarkim galima prideti dali
	{
		priceWithFurtherProductsAdded += products[level1].pricePerWeight * remainingCapacity;
	}
	
	return priceWithFurtherProductsAdded;
}
	
