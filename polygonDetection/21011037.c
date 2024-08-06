#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CYCLE 100 //Maksimum dongu sayisi
#define MAX_CYCLE_LENGTH 100 //Maksimum dongu uzunlugu
#define MAX 50 //Maksimum dosya adi uzunlugu
/*
	@brief dugumlerin yapisi
	@param vertex dugumun adini tutar
	@param weight agirlik degerini tutar
	@param next sonraki dugumu tutar
*/
typedef struct Node {
    char vertex;
    int weight;
    struct Node* next;
} Node;

/*
    @brief komsuluk listesinin yapisi
    @param head bas dugumu tutar
*/
typedef struct {
    Node* head;
} AdjList;

/*
    @brief graf yapisi
    @param numVertices dugum sayisini tutar
    @param adjLists komsuluk listesini tutar
    @param visited ziyaret edilen dugumleri tutar
    @param parent dugumun parentini tutar
    @param vertices dugumlerin listesini tutar
*/
typedef struct {
    int numVertices;
    AdjList* adjLists;
    int* visited;
    char* parent;//dugum hangi dugumden geliyor
    char* vertices; // dugumlerin listesi
} Graph;

/*
    @brief dongu yapisi
    @param cycle dongunun dugumlerini tutar
    @param length dongu uzunlugunu tutar yani dongünün kacgen olduðunu
    @param weights agirlik degerini tutar
*/
typedef struct {
    char cycle[MAX_CYCLE_LENGTH];
    int length;
    int weights;
} Cycle;

Node* createNode(char, int);//node olusturur
Graph* createGraph(int);//graph olusturur
int getVertexIndex(Graph*, char);//graph uzerinde vertex indexini bulur
void addVertex(Graph* , char);//grafa vertex ekler
void addEdge(Graph*, char , char , int);// grafa kenar ekler
void printCycle(char*, int);//kenar listesini yazdirir
int isSameCycle(char *, int, char*, int);//iki dongunun ayni olup olmadigini kontrol eder
void DFS(Graph*, int, int, Cycle *, int*, int);//DFS algoritmasi ile dongu arar
void findCycles(Graph* , Cycle *, int*);//dongu arar ve dfs cagirir
void swapCycles(Cycle* , Cycle*);//iki donguyu yer degistirir
void bubbleSortCycles(Cycle*, int);  //donguleri siralar
void swap(char* , int , int);//elemanlari yer degistirir
void bubbleSort(char* ,int);//donguyu siralar
void readFromFile(char*, Graph*);//dosyadan okuma yapar ve grafa kenar ekler

int main(){
    int vertices, edges;
    printf("Dugum sayisini giriniz: ");
    scanf("%d", &vertices);
    printf("Kenar sayisini giriniz: ");
    scanf("%d", &edges);
    Graph* graph = createGraph(vertices);
    
    char *fileName=(char*)malloc(MAX*sizeof(char));
    printf("Dosya adini giriniz: ");
    scanf(" %s", fileName);
    
    readFromFile(fileName, graph);

    Cycle *cycles = (Cycle*)malloc(MAX_CYCLE * sizeof(Cycle));
	
    int cycleCount = 0;
    findCycles(graph, cycles, &cycleCount);//Dongu arar 
    
    int* kacgen =(int*)malloc((vertices + 1) *sizeof(int));//hangi sekilden kac tane oldugunu tutar
    int i;
    for (i = 0; i <= vertices; i++){
        kacgen[i] = 0;
    }
    printf("\nSekil sayisi: %d\n", cycleCount);
    for(i= 0; i < cycleCount; i++){//dongu kacgense onun sayisi artirilir
        kacgen[cycles[i].length]++;
    }
    for(i= 0; i <= vertices; i++){//hangi sekilden kac tane oldugu yazdirilir
        if(kacgen[i]!= 0){
            printf("%d-gen: %d\n", i, kacgen[i]);
        }
    }
    printf("\n");
    bubbleSortCycles(cycles, cycleCount);//donguleri kac gen olduguna gore siralar
    for(i = 0; i < cycleCount; i++){
        printf("%d'gen: ", cycles[i].length);
        printCycle(cycles[i].cycle, cycles[i].length);
        printf("  Uzunluk: %d\n", cycles[i].weights);
    }
	printf("\nAlfabetik siralanmis hali\n");
    for(i =0; i<cycleCount; i++){//donguleri alfabetik siralar
        bubbleSort(cycles[i].cycle, cycles[i].length);
        printf("%d'gen: ", cycles[i].length);
        printCycle(cycles[i].cycle, cycles[i].length);
        printf("  Uzunluk: %d\n", cycles[i].weights);
    }

    free(kacgen);
    free(fileName);
    free(cycles);
    free(graph->vertices);
    free(graph->visited);
    free(graph->parent);
    free(graph->adjLists);
    free(graph);
    return 0;
}
// gcc -o main yenichar.c
// ./main
/*
	@brief bir dugumun komþu dugumu eklenecegi zaman node olusturur 
	@param v vertex
	@param weight agirlik
	@param newNode yeni node
	@return newNode
*/
Node* createNode(char v, int weight){//Node olusturur 
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->vertex = v;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

/*
    @brief Graph olusturur ve baslangic degerlerini atar
    @param vertices dugum sayisi
    @param graph graf
    @return graph
*/
Graph* createGraph(int vertices){//Graph olusturur
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numVertices = vertices;
    graph->adjLists = (AdjList*)malloc(vertices * sizeof(AdjList));
    graph->visited =(int*)malloc(vertices * sizeof(int));
    graph->parent =(char*)malloc(vertices * sizeof(char));
    graph->vertices = (char*)malloc(vertices * sizeof(char));
    int i;
    for(i= 0;i<vertices; i++){
        graph->adjLists[i].head = NULL;
        graph->visited[i] = 0;
        graph->parent[i] = '\0';
        graph->vertices[i] = '\0';
    }
    return graph;
}

/*
    @brief Graph uzerinde vertex indexini bulur
    @param graph graf
    @param vertex dugum
    @return index
*/
int getVertexIndex(Graph* graph, char vertex){//dugum indexini bulur
    int i;
    for (i=0; i<graph->numVertices; i++){
        if (graph->vertices[i] == vertex){//dugum bulunduysa indexi dondurur
            return i;
        }
    }
    return -1; // Vertex bulunamadi
}

/*
    @brief Graph uzerine dugum ekler
    @param graph graf
    @param vertex dugum
    @param added flag
    @return
*/
void addVertex(Graph* graph, char vertex){//dugum ekler
    int i = 0;
    int added = 0; // dugumun eklenip eklenmedigini kontrol etmek icin flag
    while (i < graph->numVertices && !added) {
        if (graph->vertices[i] == '\0') {//dugum bos ise dugum eklenir
            graph->vertices[i] = vertex;
            added = 1; // dugum eklendi
        }
        i++;
    }
}
/*
    @brief Graph uzerine kenar ekler
    @param graph graf
    @param src kaynak dugum
    @param dest hedef dugum
    @param weight agirlik
    @param srcIndex kaynak dugum indexi
    @param destIndex hedef dugum indexi
    @return
*/
void addEdge(Graph* graph, char bas, char son, int weight){//kenar ekler
    if(getVertexIndex(graph, bas) == -1){//bas dugumun indexi bulunamadiysa dugum eklenir
        addVertex(graph, bas);
    }
    if(getVertexIndex(graph, son) == -1){//son dugumun indexi bulunamadiysa dugum eklenir
        addVertex(graph, son);
    }

    int basIndex = getVertexIndex(graph, bas);//bas dugum indexi
    int sonIndex = getVertexIndex(graph, son);//son dugum indexi

    Node* newNode = createNode(son, weight);
    newNode->next = graph->adjLists[basIndex].head;//bas dugumun komsuluk listesine eklenir
    graph->adjLists[basIndex].head = newNode;

    newNode = createNode(bas, weight);
    newNode->next = graph->adjLists[sonIndex].head;//son dugumun komsuluk listesine eklenir
    graph->adjLists[sonIndex].head = newNode;
}
/*
    @brief Graph uzerinde kenar listesini yazdirir
    @param cycle bulunan dongu
    @param length dongu uzunlugu
    @return
*/
void printCycle(char *cycle, int length){//donguyu yazdirir
    printf("%c", cycle[length - 1]);
    int i;
    for (i = 0; i < length; i++){
        printf("-%c", cycle[i]);
    }
    printf("\t");
}
/*
    @brief Iki dongunun ayni olup olmadigini kontrol eder
    @param cycle1 ilk dongu
    @param length1 ilk dongu uzunlugu
    @param cycle2 ikinci dongu
    @param length2 ikinci dongu uzunlugu
    @return 1: Ayni, 0: Farkli
*/
int isSameCycle(char *cycle1, int length1, char *cycle2, int length2){//iki dongunun ayni olup olmadigini kontrol eder
    if(length1 != length2)//dongu uzunluklari farkli ise ayni degildir
        return 0;
    int i, j;
    for(i=0; i<length1; i++){//dongulerin elemanlarini karsilastirir
        int found = 0;
        for(j=0; j<length2; j++){
            if(cycle1[i] == cycle2[j]){ //elemanlar esit ise bulundu
                found = 1;
                j= length2;
            }
            else{
            	found = 0;
			}
        }
        if(!found)//elemanlar esit degilse ayni degildir
            return 0;
    }
    return 1;
}

/*
    @brief Graph uzerinde DFS algoritmasi ile dongu arar
    @param graph graf
    @param vertexIndex dugum indexi
    @param startVertexIndex baslangic dugum indexi
    @param cycles bulunan donguler
    @param cycleCount dongu sayisi
    @param currentWeight mevcut agirlik
    @param adjList komsuluk listesi
    @param connectedVertexIndex bagli dugum indexi
    @param weight agirlik
    @param current mevcut dugum
    @param cycleLength dongu uzunlugu
    @param isUnique benzersizlik kontrolu
    @return
*/
void DFS(Graph* graph, int vertexIndex, int startVertexIndex, Cycle *cycles, int* cycleCount, int currentWeight){//DFS algoritmasi ile dongu arar
    graph->visited[vertexIndex] = 1;//ziyaret edilen dugumlerin indexini 1 yapar
    Node* adjList= graph->adjLists[vertexIndex].head;//komsuluk listesininin istenilen dugumdeki headini alir

    while (adjList != NULL) {//komsuluk listesi bos degilse donguyu arar
        int connectedVertexIndex = getVertexIndex(graph, adjList->vertex);//bagli dugum indexini alir
        int weight= adjList->weight;//agirlik degerini alir
        if (!graph->visited[connectedVertexIndex]){//dugum ziyaret edilmemisse dugumu ziyaret eder
            graph->parent[connectedVertexIndex] = graph->vertices[vertexIndex]; // dugumun parentini belirler
            DFS(graph, connectedVertexIndex, startVertexIndex, cycles, cycleCount, currentWeight + weight);//kendini cagirir ve agirlik degerini artirir
        } 
        else if (connectedVertexIndex != getVertexIndex(graph, graph->parent[vertexIndex]) && connectedVertexIndex == startVertexIndex){//ziyaret edilen dugumun parenti degilse dongu bulunmustur
            
            char current = graph->vertices[vertexIndex];//mevcut dugumu alir
            int cycleLength = 0;

            while (current != graph->vertices[startVertexIndex]){//dongu baslangic dugumune kadar donguyu olusturur
                cycles[*cycleCount].cycle[cycleLength++] =current;//dongu uzunlugu artirilir ve dongu elemanlari eklenir
                current = graph->parent[getVertexIndex(graph, current)];//mevcut dugumun parentini alir
            }
            cycles[*cycleCount].cycle[cycleLength++]= graph->vertices[startVertexIndex];//dongu baslangic dugumunu ekler

            int isUnique= 1;
            int i;
            for(i= 0; i<*cycleCount; i++){//dongulerin daha once bulunup bulunmadigini kontrol eder
                if(isSameCycle(cycles[*cycleCount].cycle, cycleLength, cycles[i].cycle, cycles[i].length)){
                    isUnique= 0;
                    i= *cycleCount;
                }
            }
            if(isUnique){//dongu daha once bulunmamÄ±ssa donguyu cycles dizisine ekler
                cycles[*cycleCount].length = cycleLength;
                cycles[*cycleCount].weights = currentWeight + weight;
                (*cycleCount)++;
            }
        }
        adjList = adjList->next;//komsuluk listesini bir sonraki dugume gecirir
    }
    graph->visited[vertexIndex]=0;//ziyaret edilen dugumun indexini 0 yapar 
}

/*
    @brief Graph uzerinde dongu arar
    @param graph graf
    @param cycles bulunan donguler
    @param cycleCount dongu sayisi
    @return
*/
void findCycles(Graph* graph, Cycle *cycles, int* cycleCount){//dongu arar
    int i;
    for(i= 0; i<graph->numVertices; i++){//dfs cagirir ve dugunlerin ziyaret edilip edilmedigini kontrol eder
        if(!graph->visited[i]){
            DFS(graph,i,i,cycles,cycleCount,0);
        }
    }
}

/*
    @brief Iki donguyu yer degistirir
    @param cycle1 ilk dongu
    @param cycle2 ikinci dongu
    @return
*/
void swapCycles(Cycle* cycle1, Cycle* cycle2){//iki donguyu yer degistirir
    Cycle temp = *cycle1;
    *cycle1 = *cycle2;
    *cycle2 = temp;
}

/*
    @brief Donguleri siralar
    @param cycles donguler
    @param cycleCount dongu sayisi
    @return
*/
void bubbleSortCycles(Cycle *cycles, int cycleCount){//donguleri uzunluklarina gore siralar
    int i, j;
    for(i=0; i<cycleCount-1; i++) {
        for(j=0; j < cycleCount-i-1; j++) {
            if(cycles[j].length > cycles[j+1].length){
                swapCycles(&cycles[j], &cycles[j+1]);
            }
        }
    }
}
/*
    @brief Elemanlari yer degistirir
    @param cycle dongu
    @param i index
    @param j index
    @return
*/
void swap(char* cycle, int i, int j){//elemanlari yer degistirir
    char temp = cycle[i];
    cycle[i] = cycle[j];
    cycle[j] = temp;
}

/*
    @brief Donguyu siralar
    @param cycle dongu
    @return
*/       
void bubbleSort(char* cycle, int n){//donguyu alfabetik siralar
    int i, j;
	for (i=0; i <(n -2); i++){
	    for (j=0; j <(n-i-2); j++){ 
	        if (cycle[j] > cycle[j+1]){
	            swap(cycle, j, j + 1);
	        }
	    }
	}		
}
        
/*
    @brief Dosyadan okuma yapar ve grafa kenar ekler
    
    @param fileName dosya adi
    @param graph graf
    @return
*/  
void readFromFile(char* fileName, Graph* graph){//dosyadan okuma yapar ve grafa kenar ekler
    char start, end;
    int length;
    FILE* data = fopen(fileName, "r");
    if (!data) {
        printf("Dosya Acilamadi!\n");
        return;
    }
    while (fscanf(data, " %c %c %d\n", &start, &end, &length) != EOF) {
        addEdge(graph, start, end, length);
    }
    fclose(data);
}
