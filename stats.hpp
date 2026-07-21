#ifndef STATS_HPP
#define STATS_HPP

// --- CONSTANTES ---

// Nota: Isso dita quantas execuções independentes o algoritmo fará.
// As regras da competição exigem ESTRITAMENTE 20 execuções 
// para garantir validade estatística. NÃO ALTERAR ESTE NÚMERO.
#define MAX_TRIALS 	20 									

// Tamanho do buffer usado para alocar o nome do arquivo de saída (ex: "stats.E-n22-k4.evrp.txt")
#define CHAR_LEN 100


// --- MÉTODOS USADOS PELO MAIN.CPP PARA LOG ---

// Nota: Chamado exatamente uma vez no início do programa. 
// Ele inicializa o array de rastreamento (perf_of_trials) e cria o arquivo .txt de saída.
void open_stats(void);									

// Nota: Chamado no final, após todas as 20 execuções terem terminado.
// Esta é a função auxiliar que calcula automaticamente a Média, Desvio Padrão, 
// Melhor (Mínimo) e Pior (Máximo) das minhas 20 execuções e anexa no arquivo .txt.
void close_stats(void);									

// Nota: Chamado ao final de cada execução individual (de 0 a 19).
// Ele recebe o melhor fitness (distância) que o AG encontrou naquela execução 
// e armazena em um array para ser processado posteriormente pelo close_stats().
void get_mean(int r, double value);						

// Nota: Chamado no final do main.cpp para limpar 
// a memória alocada dinamicamente usada pelos arrays de estatísticas.
void free_stats();										

#endif