#ifndef EVRP_HPP
#define EVRP_HPP

// --- CONSTANTES ---
#define CHAR_LEN 100
// Orçamento computacional definido na competicao
#define TERMINATION 25000*ACTUAL_PROBLEM_SIZE  	

// Nome da instância que está sendo resolvida (ex: "E-n22-k4.evrp")
extern char* problem_instance;          

// Inicializadores de orçamento e rastreamento (chamados pelo main.cpp antes de cada uma das 20 execuções)
void init_evals();					    // Zera o contador de avaliações
void init_current_best();				// Reseta o melhor fitness para infinito

// Estrutura que representa um ponto de coordenada no mapa
struct node {
  int id;
  double x;
  double y;
};

// --- PARÂMETROS ---
// Eles são preenchidos pelo EVRP.cpp durante a leitura do arquivo.
// Serão usados massivamente no heuristic.cpp para guiar o Algoritmo Genético.
extern int NUM_OF_CUSTOMERS;			// Número total de clientes (N)
extern int ACTUAL_PROBLEM_SIZE; 	// Total de nós (Clientes + Depósito + Estações)
extern int NUM_OF_STATIONS;	      // Número de estações de recarga
extern int MAX_CAPACITY;			    // Capacidade máxima de carga de um veículo (Ignorado no Trabalho 1)
extern int DEPOT;						      // ID do depósito (normalmente 0)
extern double OPTIMUM;            // O melhor resultado teórico (usado para comparação)
extern int BATTERY_CAPACITY;			// Bateria máxima de um veículo (Ignorado no Trabalho 1)
extern int MIN_VEHICLES;          // Os "k" veículos disponíveis. Crucial para a divisão mTSP!

// --- MÉTODOS  ---

// Array da rota decodificada aqui para obter distância.
// Consome 1 ponto do orçamento cada vez que é chamada.
double fitness_evaluation(int *routes, int size);		

// Útil para debugar o decodificador e ver se as rotas parecem corretas no terminal
void print_solution(int *routes, int size); 				

// Verifica se a rota respeita os limites de carga e bateria. 
void check_solution(int *routes, int size); 				

// Lê o conjunto de dados do arquivo .evrp e configura as matrizes
void read_problem(char* filename);					

// Retorna a energia consumida (Distância * Taxa de Consumo)
double get_energy_consumption(int from, int to);	

// Retorna quanta carga um cliente específico exige
int get_customer_demand(int customer);				

// Retorna a distância euclidiana entre dois nós (Consome uma fração do orçamento)
double get_distance(int from, int to);				

// Verificação rápida para saber se um ID de nó específico é uma estação de recarga
bool is_charging_station(int node);					

// Retorna a melhor distância encontrada até agora na execução atual
double get_current_best();							

// Retorna quantas avaliações o meu AG já consumiu
double get_evals();									

// Limpa a memória alocada dinamicamente ao final do programa
void free_EVRP();									

#endif