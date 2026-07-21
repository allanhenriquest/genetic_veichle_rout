#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<cstring>
#include<math.h>
#include<fstream>
#include<time.h>
#include<limits.h>

#include "EVRP.hpp"

using namespace std;

// --- VARIÁVEIS GLOBAIS ---

char* problem_instance;          // Nome da instância .evrp
struct node *node_list;          // Lista contendo ID, coordenadas X e Y de todos os nós
int *cust_demand;                // Array para armazenar a demanda de carga de cada cliente
bool* charging_station;          // Array booleano para verificar facilmente se um nó é uma estação
double **distances;              // Minha matriz 2D de distâncias (economiza CPU para não recalcular)
int problem_size;                // A dimensão bruta lida do arquivo
double energy_consumption;       // Energia consumida por unidade de distância

int DEPOT;                       // ID do depósito (geralmente 0)
int NUM_OF_CUSTOMERS;            // Número de clientes reais que preciso visitar
int ACTUAL_PROBLEM_SIZE;         // Total de nós (Clientes + Estações de Recarga + 1 Depósito)
double OPTIMUM;                  // Melhor ótimo global conhecido (usado apenas como referência)
int NUM_OF_STATIONS;             // Número total de estações de recarga disponíveis
int BATTERY_CAPACITY;            // Energia máxima que as baterias dos veículos podem armazenar
int MAX_CAPACITY;                // Capacidade máxima de carga dos veículos
int MIN_VEHICLES;                // O número de veículos disponíveis (k) - super importante para o mTSP!

// --- RASTREAMENTO DO ORÇAMENTO ---
double evals;                    // Quantas vezes avaliei uma solução. DEVO respeitar o limite do orçamento!
double current_best;             // O melhor fitness (menor distância) que meu algoritmo encontrou até agora


/****************************************************************/
/* Calcula e retorna a distância euclidiana entre dois objetos  */
/****************************************************************/
/* Nota: Esta é uma fórmula padrão do Teorema de Pitágoras.
 * Utilizada apenas uma vez durante a configuração para preencher minha matriz. 
 */
double euclidean_distance(int i, int j) {
  double xd, yd;
  double r = 0.0;
  xd = node_list[i].x - node_list[j].x;
  yd = node_list[i].y - node_list[j].y;
  r  = sqrt(xd*xd + yd*yd);
  return r;
}

/****************************************************************/
/* Calcula a matriz de distâncias da instância do problema      */
/****************************************************************/
/* Nota: Em vez de calcular distâncias em tempo real durante os 
 * loops do Algoritmo Genético (o que seria muito lento), 
 * calcula todas as distâncias possíveis aqui no início e salva.
 */
void compute_distances(void) {
  int i, j;
  for(i = 0; i < ACTUAL_PROBLEM_SIZE; i++){
    for(j = 0; j < ACTUAL_PROBLEM_SIZE; j++){
      distances[i][j] = euclidean_distance(i,j);
    }
  }
}

/****************************************************************/
/* Gera e retorna uma matriz 2D do tipo double                  */
/****************************************************************/
// Função auxiliar para alocar memória para a matriz de distâncias.
double ** generate_2D_matrix_double(int n, int m){
  double **matrix;

  matrix = new double*[n];
  for ( int i = 0 ; i < n ; i++ ) {
    matrix[i] = new double[m];
  }
  // Inicializar tudo com 0.0 para evitar lixo de memória.
  for(int i = 0; i < n; i++){
    for(int j = 0; j < m; j++) {
      matrix[i][j] = 0.0;
    }
  }
  return matrix;
}


/****************************************************************/
/* Lê a instância do problema e gera o vetor de objetos inicial */
/****************************************************************/
/* Nota: Este é o interpretador. Ele lê o arquivo .evrp linha 
 * por linha, busca palavras-chave específicas e preenche as variáveis.
 * É um código padrão da competição, e essencialmente mapeia 
 * o arquivo de texto para as estruturas C++.
 */
void read_problem(char* filename){
  int i;
  char line[CHAR_LEN];
  char * keywords;
  char Delimiters[] = " :=\n\t\r\f\v";
  ifstream fin(filename);
  
  while((fin.getline(line, CHAR_LEN-1))){

    if(!(keywords = strtok(line, Delimiters)))
      continue;
      
    if(!strcmp(keywords, "DIMENSION")){
      if(!sscanf(strtok(NULL, Delimiters), "%d", &problem_size)){
	    cout<<"Erro no DIMENSION"<<endl;
	    exit(0);
      }
    }
    else if(!strcmp(keywords, "EDGE_WEIGHT_TYPE")){
      char * tempChar;
      if(!(tempChar=strtok(NULL, Delimiters))){
	    cout<<"Erro no EDGE_WEIGHT_TYPE"<<endl;
	    exit(0);
      }
      if(strcmp(tempChar, "EUC_2D")){
	    cout<<"Não é EUC_2D"<<endl;
	    exit(0);
      }
    }
    else if (!strcmp(keywords, "CAPACITY")){
       if(!sscanf(strtok(NULL,Delimiters), "%d", &MAX_CAPACITY)){
          cout << "Erro no CAPACITY" << endl;
          exit(0);
       }
    }
    else if (!strcmp(keywords, "VEHICLES")){
       if(!sscanf(strtok(NULL,Delimiters), "%d", &MIN_VEHICLES)){
          cout << "Erro no VEHICLES" << endl;
          exit(0);
       }
    }
    else if (!strcmp(keywords, "ENERGY_CAPACITY")){
       if(!sscanf(strtok(NULL,Delimiters), "%d", &BATTERY_CAPACITY)){
          cout << "Erro no ENERGY_CAPACITY" << endl;
          exit(0);
       }
    }
    else if (!strcmp(keywords, "ENERGY_CONSUMPTION")){
       if(!sscanf(strtok(NULL,Delimiters), "%lf", &energy_consumption)){
          cout << "Erro no ENERGY_CONSUMPTION" << endl;
          exit(0);
       }
    }
    else if (!strcmp(keywords, "STATIONS")){
       if(!sscanf(strtok(NULL,Delimiters), "%d", &NUM_OF_STATIONS)){
          cout << "Erro no STATIONS" << endl;
          exit(0);
       }
    }
    else if (!strcmp(keywords, "OPTIMAL_VALUE")){
       if(!sscanf(strtok(NULL,Delimiters), "%lf", &OPTIMUM)){
          cout << "Erro no OPTIMAL_VALUE" << endl;
          exit(0);
       }
    }
    else if(!strcmp(keywords, "NODE_COORD_SECTION")){
      if(problem_size!=0){
          // O tamanho do problema é clientes + depósito. Subtrair 1 para ter apenas os clientes.
          NUM_OF_CUSTOMERS = problem_size - 1;
          ACTUAL_PROBLEM_SIZE = problem_size + NUM_OF_STATIONS;
        
          node_list = new node[ACTUAL_PROBLEM_SIZE];
      
        for(i=0; i < ACTUAL_PROBLEM_SIZE; i++){
	       // Armazenando objetos iniciais (ID, X, Y)
           fin>>node_list[i].id;
	       fin>>node_list[i].x>>node_list[i].y;
           node_list[i].id -=1; // Ajusta para índice 0
        }
        // com as coordenadas, é possível construir a matriz de distâncias
        distances = generate_2D_matrix_double(ACTUAL_PROBLEM_SIZE, ACTUAL_PROBLEM_SIZE);
     } else {
           cout << "Arquivo de instância do problema incorreto" << endl;
           exit(1);
     }
    }
    else if(!strcmp(keywords, "DEMAND_SECTION")){
     if(problem_size!=0){
       int temp;
       cust_demand = new int[ACTUAL_PROBLEM_SIZE];
       charging_station = new bool[ACTUAL_PROBLEM_SIZE];
       
       for(i = 0; i < problem_size; i++){
        fin >> temp;
        fin >> cust_demand[temp-1];
       }

       // configuração das flags das estações de recarga.
       // O depósito será marcado como estação em DEPOT_SECTION.
       for(i = 0; i < ACTUAL_PROBLEM_SIZE; i++){
          if(i < problem_size) {
            charging_station[i] = false;
          } else {
            charging_station[i] = true;
            cust_demand[i] = 0;
          }
        }
      }
    }
     else if(!strcmp(keywords, "DEPOT_SECTION")){
      fin >> DEPOT;
      DEPOT-=1;
      charging_station[DEPOT] = true;
    } 

  }
  fin.close();
  if(ACTUAL_PROBLEM_SIZE == 0) {
        cout << "Arquivo de instância do problema incorreto" << endl;
        exit(1);
  } else {
    compute_distances(); // Calcula todas as distâncias Euclidianas
  }
}


/****************************************************************/
/* Avalia o fitness da solução gerada pelo AG                   */
/****************************************************************/

double fitness_evaluation(int *routes, int size) {
  int i;
  double tour_length = 0.0;
  
  // Formato esperado:
  // Sequência de nós: [DEPÓSITO, 5, 6, 8, DEPÓSITO, 1, 2, 3, 4, DEPÓSITO, 7, DEPÓSITO]
  
  for (i = 0; i < size-1; i++) 
    tour_length += distances[routes[i]][routes[i+1]];

  // Se o  AG encontrou uma rota mais curta que a melhor vista, salvar.
  if(tour_length < current_best)
      current_best = tour_length;

  // IMPORTANTE: Isso conta contra meu orçamento computacional!
  // Deve-se parar o AG quando 'evals' atingir o limite de TERMINATION.
  evals++; 

  return tour_length;
}

/****************************************************************/
/* Imprime as rotas da solução para fins de debug               */
/****************************************************************/
void print_solution(int *routes, int size) {
    int i;
    // Apenas um loop simples para imprimir o array. Útil para debugar o decodificador.
    for( i = 0 ; i < size; i++ ) {
         cout << routes[i] <<  " , ";
    }  
}

/****************************************************************/
/* Valida as rotas (Restrições de Capacidade e Energia)         */
/****************************************************************/
void check_solution(int *t, int size){
  int i, from, to;
  double energy_temp = BATTERY_CAPACITY; 
  double capacity_temp = MAX_CAPACITY;
  double distance_temp = 0.0;

  for(i = 0; i < size-1; i++){
    from = t[i];
    to = t[i+1];
    capacity_temp -= get_customer_demand(to);
    energy_temp -= get_energy_consumption(from,to);
    distance_temp += get_distance(from,to);
    
    if(capacity_temp < 0.0) {
      cout << "Erro: capacidade abaixo de 0 no cliente " << to <<  endl;
      print_solution(t,size);
      exit(1);
    }
    if(energy_temp < 0.0) {
       cout << "Erro: energia abaixo de 0 de " << from << " para " << to <<  endl;
       print_solution(t,size);
       exit(1);
    }
    if(to == DEPOT) {
      capacity_temp = MAX_CAPACITY;
    }
    if(is_charging_station(to)==true || to==DEPOT){
      energy_temp = BATTERY_CAPACITY;
    }
  }
  if(distance_temp != fitness_evaluation(t,size)) {
    cout << "Erro: verifique a avaliação do fitness" << endl;
  }
}

/****************************************************************/
/* Retorna a distância entre dois pontos                        */
/****************************************************************/
/* Nota: Isso é útil para Busca Local (como 2-opt), onde
 * apenas calcula-se o delta de uma pequena alteração 
 * sem avaliar todo o array da rota. 
 * Adiciona apenas uma fração de avaliação ao orçamento.
 */
double get_distance(int from, int to){
  evals += (1.0/ACTUAL_PROBLEM_SIZE); 
  return distances[from][to];
}

/****************************************************************/
/* Retorna a energia consumida viajando entre dois pontos       */
/****************************************************************/
double get_energy_consumption(int from, int to) {
    /* Nota: NÃO ADICIONAR ISSO AO CUSTO DA ROTA. 
       O custo é medido puramente em distância. */
    return energy_consumption*distances[from][to];
}

/****************************************************************/
/* Retorna a demanda de carga para um cliente específico        */
/****************************************************************/
int get_customer_demand(int customer){
  return cust_demand[customer];
}

/****************************************************************/
/* Retorna true se o nó for uma estação de recarga              */
/****************************************************************/
bool is_charging_station(int node){
  bool flag = false; 
  if(charging_station[node] == true)
    flag = true; 
  else 
    flag = false;
  return flag;
}

/****************************************************************/
/* Getters e Inicializadores para rastreamento de estatísticas  */
/****************************************************************/

// Retorna a melhor qualidade de solução encontrada na execução atual
double get_current_best(){
 return current_best;
}

// Zera a melhor solução para uma nova execução independente (chamado pelo main.cpp)
void init_current_best(){
   current_best = INT_MAX;
}

// Retorna quantas avaliações o meu AG já consumiu
double get_evals(){
  return evals;
}

// Zera o contador de orçamento computacional para uma nova execução
void init_evals(){
  evals = 0;
}

/****************************************************************/
/* Limpa a memória alocada para prevenir vazamentos             */
/****************************************************************/
void free_EVRP(){
  int i;

  delete[] node_list;
  delete[] cust_demand;
  delete[] charging_station;

  // Devo deletar a matriz 2D de distâncias iterando corretamente pelas linhas
  for(i = 0; i < ACTUAL_PROBLEM_SIZE; i++) {
    delete[] distances[i];
  }
  delete[] distances;
}