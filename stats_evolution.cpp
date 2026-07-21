#include <iostream>
#include <fstream>
#include <cmath>
#include <numeric>
#include <string>
#include <map>
#include "stats_evolution.hpp"

using namespace std;

static string current_trace_filename = "evolution_trace.csv";

/****************************************************************/
/* Configura o ficheiro de gravação para a run atual            */
/****************************************************************/
void init_evolution_file(int run, const char* instance_name) {
    // Cria nomes como "evolution_trace_E-n22-k4.evrp_run_1.csv"
    current_trace_filename = string("evolution_trace_") + string(instance_name) + "_run_" + to_string(run) + ".csv";
    // Abre em modo trunc para limpar execuções anteriores com o mesmo nome
    ofstream fout(current_trace_filename, ios::trunc); 
    if (fout.is_open()) fout.close();
}

/****************************************************************/
/* Calcula o desvio padrão e a entropia genética                */
/****************************************************************/
PopStats calculate_population_stats(const std::vector<Chromosome>& pop) {
    PopStats stats = {0.0, 0.0};
    if (pop.empty()) return stats;

    // 1. Calcula Desvio Padrão do Fitness (Diversidade de Qualidade)
    double sum_fitness = 0.0;
    for (const auto& ind : pop) {
        sum_fitness += ind.fitness;
    }
    double mean_fitness = sum_fitness / pop.size();

    double variance_sum = 0.0;
    for (const auto& ind : pop) {
        variance_sum += (ind.fitness - mean_fitness) * (ind.fitness - mean_fitness);
    }
    stats.fitness_std_dev = sqrt(variance_sum / pop.size());

    // 2. Calcula Entropia Genética (Diversidade Estrutural / Shannon)
    int num_genes = pop[0].permutation.size();
    double total_entropy = 0.0;

    for (int pos = 0; pos < num_genes; ++pos) {
        map<int, int> allele_counts;
        for (const auto& ind : pop) {
            allele_counts[ind.permutation[pos]]++;
        }
        
        double pos_entropy = 0.0;
        for (const auto& pair : allele_counts) {
            double p = (double)pair.second / pop.size();
            pos_entropy -= p * log2(p);
        }
        total_entropy += pos_entropy;
    }
    
    stats.gene_entropy = total_entropy / num_genes;

    return stats;
}

/****************************************************************/
/* Grava as estatísticas no CSV                                 */
/****************************************************************/
void log_generation_stats(int gen, PopStats stats) {
    ofstream fout(current_trace_filename, ios::app);
    if (fout.is_open()) {
        fout << gen << "," << stats.fitness_std_dev << "," << stats.gene_entropy << "\n";
        fout.close();
    }
}