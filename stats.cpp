#include<cmath>
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
#include "stats.hpp"

using namespace std;

// --- VARIÁVEIS GLOBAIS PARA ESTATÍSTICAS ---
// Nota: Estas variáveis lidam com o arquivo de saída e armazenam os resultados 
// das minhas 20 execuções independentes para calcular as métricas finais.

FILE *log_performance;           // Ponteiro do arquivo para meu output .txt
char *perf_filename;             // String dinâmica que contém o nome do arquivo de saída
double* perf_of_trials;          // Array para armazenar o melhor fitness de cada uma das 20 execuções


/****************************************************************/
/* Abre o arquivo e aloca memória para o rastreamento das runs  */
/****************************************************************/
void open_stats(void){
    // Inicializa o array que conterá os melhores resultados dos meus 20 ensaios
    perf_of_trials = new double[MAX_TRIALS];

    for(int i = 0; i < MAX_TRIALS; i++){
        perf_of_trials[i] = 0.0;
    }

  // Inicializa e abre o arquivo de saída
  // Formato o nome do arquivo para incluir o nome da instância (ex: "stats.E-n22-k4.evrp.txt")
  perf_filename = new char[CHAR_LEN];
  sprintf(perf_filename, "stats.%s.txt", problem_instance);
  
  // Abre o arquivo no modo "append" ("a" - anexar). Se falhar, encerra o programa.
  if ((log_performance = fopen(perf_filename,"a")) == NULL) { 
      exit(2); 
  }
}

/****************************************************************/
/* Salva o melhor fitness de uma única execução                 */
/****************************************************************/
// Nota: main.cpp chama isto no final de cada execução. 
// 'r' é o índice da run (0 a 19), e 'value' é o melhor fitness do meu AG.
void get_mean(int r, double value) {
  perf_of_trials[r] = value;
}

/****************************************************************/
/* Auxiliar: Calcula a Média                                    */
/****************************************************************/
// Nota: Função padrão para calcular a média das minhas 20 execuções.
double mean(double* values, int size){
  int i;
  double m = 0.0;
  for (i = 0; i < size; i++){
      m += values[i];
  }
  m = m / (double)size;
  return m; // média
}

/****************************************************************/
/* Auxiliar: Calcula o Desvio Padrão                            */
/****************************************************************/
// Nota: Isso mede a consistência do o AG. Um desvio padrão menor 
// significa que o algoritmo é robusto e converge para resultados similares 
// independentemente da semente aleatória inicial.
double stdev(double* values, int size, double average){
  int i;
  double dev = 0.0;

  if (size <= 1)
    return 0.0;
  for (i = 0; i < size; i++){
    dev += ((double)values[i] - average) * ((double)values[i] - average);
  }
  return sqrt(dev / (double)(size - 1)); // desvio padrão
}

/****************************************************************/
/* Auxiliar: Encontra o Mínimo (Melhor de todas as Runs)        */
/****************************************************************/
// Nota: Isso encontra a melhor solução absoluta que o AG já produziu
// em todas as 20 execuções independentes.
double best_of_vector(double *values, int l ) {
  double min;
  int k;
  k = 0;
  min = values[k];
  for( k = 1 ; k < l ; k++ ) {
    if( values[k] < min ) {
      min = values[k];
    }
  }
  return min;
}

/****************************************************************/
/* Auxiliar: Encontra o Máximo (A Pior das Runs)                */
/****************************************************************/
// Nota: Encontra o pior desempenho entre as  20 execuções.
double worst_of_vector(double *values, int l ) {
  double max;
  int k;
  k = 0;
  max = values[k];
  for( k = 1 ; k < l ; k++ ) {
    if( values[k] > max ){
      max = values[k];
    }
  }
  return max;
}

/****************************************************************/
/* Finaliza as estatísticas e as escreve no arquivo .txt        */
/****************************************************************/
void close_stats(void){
  int i;
  double perf_mean_value, perf_stdev_value;
 
  // Primeiro, imprime o melhor fitness exato alcançado em cada uma das 20 runs
  for(i = 0; i < MAX_TRIALS; i++){
    fprintf(log_performance, "%.2f", perf_of_trials[i]);
    fprintf(log_performance,"\n");
  }

  // Calcula estatísticas de resumo para a apresentação
  perf_mean_value = mean(perf_of_trials, MAX_TRIALS);
  perf_stdev_value = stdev(perf_of_trials, MAX_TRIALS, perf_mean_value);
  
  // Escreve a Média e o Desvio Padrão
  fprintf(log_performance,"Média %f\t ", perf_mean_value);
  fprintf(log_performance,"\tDesvio Padrão %f\t ", perf_stdev_value);
  fprintf(log_performance,"\n");
  
  // Escreve o Melhor valor (Mínimo) encontrado
  fprintf(log_performance, "Mínimo: %f\t ", best_of_vector(perf_of_trials, MAX_TRIALS));
  fprintf(log_performance,"\n");
  
  // Escreve o Pior valor (Máximo) encontrado
  fprintf(log_performance, "Máximo: %f\t ", worst_of_vector(perf_of_trials, MAX_TRIALS));
  fprintf(log_performance,"\n");

  // Fecha o fluxo do arquivo
  fclose(log_performance);
}

/****************************************************************/
/* Limpa os arrays alocados dinamicamente                       */
/****************************************************************/
void free_stats(){
  // Libera a memória para evitar vazamentos!
  delete[] perf_of_trials;
  delete[] perf_filename; 
}