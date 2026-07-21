#include<iostream>
#include<stdlib.h>
#include<limits.h>

#include "EVRP.hpp"
#include "heuristic.hpp"
#include "stats.hpp"

using namespace std;

/****************************************************************/
/* Inicializa as variáveis antes de cada execução independente  */
/****************************************************************/
/* Nota: O framework exige exatamente 20 execuções independentes.
 * Esta função define a semente aleatória com base no número da run 
 * para que o AG tenha um ponto de partida diferente a cada vez. 
 * Também zera o orçamento de avaliações.
 */
void start_run(int r){
  srand(r); // Define uma semente aleatória diferente para cada uma das 20 execuções
  init_evals(); // Zera o contador de orçamento (evals)
  init_current_best(); // Reseta o current_best fitness para infinito
  cout << "Execução: " << r << " com semente aleatória " << r << endl;
}

/****************************************************************/
/* Captura os resultados no fim de cada execução independente   */
/****************************************************************/
/* Nota: Quando uma execução termina (porque atingiu o orçamento),
 * esta função pega o melhor fitness que o AG encontrou e passa 
 * para o módulo de estatísticas para ser registrado.
 */
void end_run(int r){
  get_mean(r-1, get_current_best()); // Armazena o resultado no stats.cpp
  cout << "Fim da execução " << r << " com qualidade da melhor solução " << get_current_best() 
       << " total de avaliações: " << get_evals()  << endl;
  cout << " " << endl;
}

/****************************************************************/
/* Verifica se o orçamento computacional foi esgotado           */
/****************************************************************/
/* Nota: Esta é a condição de parada crítica. 
 * Enquanto isto retornar falso, o loop while no main() continuará 
 * chamando o run_heuristic() (o ciclo de gerações do AG).
 */
bool termination_condition(void) {
  bool flag; 
  if(get_evals() >= TERMINATION) 
    flag = true;
  else
    flag = false;

  return flag;
}

/****************************************************************/
/* Função Principal - O Controlador                             */
/****************************************************************/
int main(int argc, char *argv[]) {
  int run;
  
  /* Passo 1: Ler o arquivo de instância */
  // Nota: O usuário deve passar o nome do arquivo .evrp como argumento no terminal.
  // Exemplo: ./main E-n22-k4.evrp
  problem_instance = argv[1];       
  read_problem(problem_instance);   // Analisa o arquivo e cria a matriz de distâncias

  /* Passo 2: Preparar o rastreador de estatísticas */
  open_stats(); 

  /* Passo 3: Iterar pelas 20 execuções independentes */
  // MAX_TRIALS é definido como 20 no stats.hpp
  for(run = 1; run <= MAX_TRIALS; run++){
      
      start_run(run); // Reseta tudo para esta execução
      
      // Inicializo a população do Algoritmo Genético aqui
      initialize_heuristic(); 

      /* Passo 4: O Loop de Evolução */
      // Nota: Isso continuará executando as gerações do AG
      // até que as avaliações de fitness atinjam o limite de 25.000 * N.
      while(!termination_condition()){
        run_heuristic();  
      }
      
      /* Passo 5: Registrar o melhor cromossomo desta execução */
      end_run(run);  
  }
  
  /* Passo 6: Gerar o relatório final em .txt */
  close_stats(); // Calcula a Média, Desvio Padrão, Mínimo e Máximo

  // Passo 7: Limpar toda a memória alocada dinamicamente
  free_stats();
  free_heuristic();
  free_EVRP();

  return 0;
}