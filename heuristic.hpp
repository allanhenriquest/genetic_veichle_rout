#ifndef HEURISTIC_HPP
#define HEURISTIC_HPP

// --- ESTRUTURA DA SOLUÇÃO ---
// Nota: Esta estrutura é estritamente exigida pelo framework. 
// Representa o formato decodificado final de uma solução que o EVRP.cpp 
// pode entender e avaliar. Embora o  Algoritmo Genético use 
// a sua própria estrutura 'Chromosome' internamente, DEVO traduzir os 
// melhores cromossomos neste formato 'solution' para o rastreamento do framework.
struct solution{
  int *tour;	        // Array contendo a sequência de nós visitados
  int id;               // ID da solução (usado principalmente para rastreamento)
  double tour_length;   // O valor do fitness (qualidade/distância da solução)
  int steps;            // O tamanho total do array de rota (nós + depósitos)
  
  // Nota: O formato do array *tour DEVE ser da seguinte forma:
  // *tour:  0 - 5 - 6 - 8 - 0 - 1 - 2 - 3 - 4 - 0 - 7 - 0
  // *steps: 12
  // Este exemplo consiste em 3 rotas retornando ao depósito (0): 
  // Rota 1: 0 - 5 - 6 - 8 - 0
  // Rota 2: 0 - 1 - 2 - 3 - 4 - 0
  // Rota 3: 0 - 7 - 0
};

// --- MELHOR SOLUÇÃO GLOBAL ---
// Nota: O framework (main.cpp e stats.cpp) espera que este 
// ponteiro contenha sempre a melhor solução geral encontrada até agora. 
// Atualizar dentro de run_heuristic() sempre que o AG encontrar um melhor fitness.
extern solution *best_sol;

#include <vector>

// --- ESTRUTURAS DE DADOS INTERNAS ---
// Movido para cá para que stats_evolution possa acessar
struct Chromosome {
    std::vector<int> permutation; 
    double fitness;
};

// --- FUNÇÕES DO CICLO DE VIDA DA HEURÍSTICA ---

// Nota: Chamado uma vez por execução no main.cpp. 
void initialize_heuristic();

// Nota: Chamado repetidamente num loop while pelo main.cpp até esgotar o orçamento.
// ISTO representa UMA geração do Algoritmo Genético.
// Lógica interna: Avaliar fitness -> Selecionar -> Crossover -> Mutação -> Atualizar População.
void run_heuristic();

// Nota: Chamado no final do main.cpp.
// Usarei isso para liberar os arrays alocados dinamicamente (como best_sol->tour).
void free_heuristic();

#endif