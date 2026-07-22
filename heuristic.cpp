#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits.h>
#include <random>
#include "EVRP.hpp"
#include "heuristic.hpp"
#include "stats_evolution.hpp"

using namespace std;

solution *best_sol;

// --- PARÂMETROS DO ALGORITMO GENÉTICO ---
const int POP_SIZE = 100;
const int TOURNAMENT_SIZE = 5;
const double CROSSOVER_RATE = 0.8;

// --- VARIÁVEIS DE CONTROLE DINÂMICO E RASTREAMENTO ---
double current_mutation_rate = 0.1; // Taxa de mutação inicial (10%)
int stagnation_counter = 0;         // Contador de gerações sem melhoria
double last_best_fitness = INT_MAX; // Último melhor fitness registrado

int current_run_id = 1;             // Identificador da execução (run) atual
int current_generation = 0;         // Contador de gerações da execução atual

vector<Chromosome> population;

// --- DECODIFICADOR mTSP ---
void decode_chromosome_to_solution(const Chromosome &chrom, int *tour_array, int &steps) {
    steps = 0;
    tour_array[steps++] = DEPOT;

    int num_vehicles = MIN_VEHICLES;
    int clients_per_vehicle = NUM_OF_CUSTOMERS / num_vehicles;
    int extra_clients = NUM_OF_CUSTOMERS % num_vehicles;

    int current_client_idx = 0;

    for (int v = 0; v < num_vehicles; ++v) {
        int clients_to_assign = clients_per_vehicle + (extra_clients > 0 ? 1 : 0);
        if (extra_clients > 0) extra_clients--;

        for (int i = 0; i < clients_to_assign; ++i) {
            if (current_client_idx < NUM_OF_CUSTOMERS) {
                tour_array[steps++] = chrom.permutation[current_client_idx++];
            }
        }
        tour_array[steps++] = DEPOT;
    }
}

// --- ORDER CROSSOVER (OX) ---
Chromosome order_crossover(const Chromosome &p1, const Chromosome &p2) {
    int size = p1.permutation.size();
    Chromosome child;
    child.permutation.resize(size, -1);
    child.fitness = INT_MAX;

    int start = rand() % size;
    int end = rand() % size;
    if (start > end) swap(start, end);

    // Copia a fatia do Pai 1
    for (int i = start; i <= end; ++i) {
        child.permutation[i] = p1.permutation[i];
    }

    // Preenche com o Pai 2
    int current_pos = (end + 1) % size;
    for (int i = 0; i < size; ++i) {
        int p2_gene = p2.permutation[(end + 1 + i) % size];
        
        bool found = false;
        for (int j = start; j <= end; ++j) {
            if (child.permutation[j] == p2_gene) {
                found = true;
                break;
            }
        }

        if (!found) {
            child.permutation[current_pos] = p2_gene;
            current_pos = (current_pos + 1) % size;
        }
    }
    return child;
}

// --- INICIALIZAÇÃO ---
void initialize_heuristic(){
    best_sol = new solution;
    best_sol->tour = new int[ACTUAL_PROBLEM_SIZE * 2]; 
    best_sol->id = 1;
    best_sol->steps = 0;
    best_sol->tour_length = INT_MAX;

    // Reseta as variáveis de controle para uma nova run independente
    current_mutation_rate = 0.1;
    stagnation_counter = 0;
    last_best_fitness = INT_MAX;
    current_generation = 0;
    
    // Inicializa o arquivo de rastro para a execução atual
    init_evolution_file(current_run_id++, problem_instance);
    
    population.clear();
    
    // Cria a permutação base (ex: 1 a N)
    vector<int> base_permutation;
    for (int i = 1; i <= NUM_OF_CUSTOMERS; ++i) {
        base_permutation.push_back(i);
    }

    mt19937 rng(rand());

    for (int i = 0; i < POP_SIZE; ++i) {
        Chromosome ind;
        ind.permutation = base_permutation;
        shuffle(ind.permutation.begin(), ind.permutation.end(), rng);
        ind.fitness = INT_MAX; // Marca como não avaliado
        population.push_back(ind);
    }
}

// --- MOTOR DO ALGORITMO GENÉTICO (POR GERAÇÃO) ---
void run_heuristic(){
    
    /* 1. FASE DE AVALIAÇÃO COM TRAVA DE SEGURANÇA */
    double current_gen_best_fitness = INT_MAX;
    int best_idx = -1;

    for (int i = 0; i < POP_SIZE; ++i) {
        // Se a população ainda não tem fitness avaliado
        if (population[i].fitness == INT_MAX) {
            
            // TRAVA DE SEGURANÇA: Se já atingimos o orçamento exato, interrompe!
            if (get_evals() >= TERMINATION) {
                break; // Sai do loop de avaliação sem consumir mais orçamento
            }

            int temp_tour[ACTUAL_PROBLEM_SIZE * 2];
            int temp_steps = 0;
            
            decode_chromosome_to_solution(population[i], temp_tour, temp_steps);
            // Avalia e desconta do orçamento computacional
            population[i].fitness = fitness_evaluation(temp_tour, temp_steps);
        }

        if (population[i].fitness < current_gen_best_fitness) {
            current_gen_best_fitness = population[i].fitness;
            best_idx = i;
        }
    }

    // Se a trava de segurança barrou tudo e ninguém foi avaliado direito, encerra a função
    if (best_idx == -1) return;

    // Atualiza o Melhor Global do Programa
    if (current_gen_best_fitness < best_sol->tour_length) {
        best_sol->tour_length = current_gen_best_fitness;
        decode_chromosome_to_solution(population[best_idx], best_sol->tour, best_sol->steps);
    }

    /* 2. GRAVAÇÃO DE ESTATÍSTICAS */
    // Grava apenas a cada 50 gerações para manter os arquivos CSVs leves
    if (current_generation % 50 == 0) {
        PopStats current_stats = calculate_population_stats(population);
        log_generation_stats(current_generation, current_stats);
    }

    /* 3. CONTROLE ADAPTATIVO DE PARÂMETROS (EIBEN) */
    if (current_gen_best_fitness < last_best_fitness) {
        last_best_fitness = current_gen_best_fitness;
        stagnation_counter = 0; // Progresso detectado! Reseta a estagnação.
    } else {
        stagnation_counter++;   // Estagnou.
    }

    // Choque Dinâmico para escapar de Ótimos Locais
    if (stagnation_counter >= 50) {
        current_mutation_rate = 0.50; // 50% Exploração agressiva
    } else {
        current_mutation_rate = 0.10; // 10% Ajuste Fino (Explotação)
    }

    /* 4. FASE DE REPRODUÇÃO */
    vector<Chromosome> next_generation;

    // Elitismo: Copia os 2 melhores (Garante monotonicidade)
    sort(population.begin(), population.end(), [](const Chromosome &a, const Chromosome &b) {
        return a.fitness < b.fitness;
    });
    next_generation.push_back(population[0]);
    next_generation.push_back(population[1]);

    while (next_generation.size() < POP_SIZE) {
        // Seleção Pai 1 (Torneio)
        Chromosome parent1 = population[rand() % POP_SIZE];
        for (int i = 1; i < TOURNAMENT_SIZE; ++i) {
            Chromosome competitor = population[rand() % POP_SIZE];
            if (competitor.fitness < parent1.fitness) parent1 = competitor;
        }

        // Seleção Pai 2 (Torneio)
        Chromosome parent2 = population[rand() % POP_SIZE];
        for (int i = 1; i < TOURNAMENT_SIZE; ++i) {
            Chromosome competitor = population[rand() % POP_SIZE];
            if (competitor.fitness < parent2.fitness) parent2 = competitor;
        }

        Chromosome child = parent1;
        
        // Recombinação OX
        if ((rand() / (double)RAND_MAX) < CROSSOVER_RATE) {
            child = order_crossover(parent1, parent2);
        }
        
        // Mutação Swap (Taxa Adaptativa)
        if ((rand() / (double)RAND_MAX) < current_mutation_rate) {
            int pos1 = rand() % NUM_OF_CUSTOMERS;
            int pos2 = rand() % NUM_OF_CUSTOMERS;
            swap(child.permutation[pos1], child.permutation[pos2]);
            // Importante: Reseta o fitness para forçar a avaliação na próxima geração
            child.fitness = INT_MAX; 
        }
        
        next_generation.push_back(child);
    }
    
    // Atualiza a População
    population = next_generation;
    current_generation++;
}

// --- LIMPEZA DE MEMÓRIA ---
void free_heuristic(){
    delete[] best_sol->tour;
    delete best_sol;
}