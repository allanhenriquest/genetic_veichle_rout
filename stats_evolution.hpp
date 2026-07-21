#ifndef STATS_EVOLUTION_HPP
#define STATS_EVOLUTION_HPP

#include <vector>
#include "heuristic.hpp"

// Estrutura para armazenar as métricas populacionais
struct PopStats {
    double fitness_std_dev; // Diversidade de fitness
    double gene_entropy;    // Incerteza estrutural das rotas
};

// Funções para monitorização
void init_evolution_file(int run, const char* instance_name);
PopStats calculate_population_stats(const std::vector<Chromosome>& pop);
void log_generation_stats(int gen, PopStats stats);

#endif