# 🚚⚡ Simulador mTSP com Algoritmo Genético

## Otimização de Rotas para Veículos Elétricos utilizando Computação Evolucionista

Este projeto apresenta uma implementação em **C++ de um Algoritmo Genético (AG)** para resolver uma versão relaxada do **Electric Vehicle Routing Problem (EVRP)**, modelada como um **Multiple Traveling Salesman Problem (mTSP)**.

O objetivo é investigar a capacidade de algoritmos evolutivos em encontrar soluções eficientes para problemas de roteamento de múltiplos veículos, preservando diversidade populacional e respeitando um orçamento computacional rigoroso.

Além do núcleo evolutivo implementado em C++, o projeto inclui ferramentas em **Python** para análise estatística, monitoramento da dinâmica evolutiva e geração automática de gráficos comparativos.

---

## 📌 Características do Projeto

### ✅ Modelo de Problema

O EVRP original foi simplificado para permitir o estudo isolado do comportamento evolutivo do algoritmo.

#### Restrições Ativas

* Cada cliente deve ser visitado exatamente uma vez.
* Todas as rotas começam e terminam no depósito.
* Número fixo de veículos (frota k).
* Soluções representadas por permutações válidas.

#### Restrições Relaxadas

* Capacidade de carga dos veículos.
* Consumo energético da bateria.
* Estações de recarga.
* Autonomia máxima dos veículos.

Dessa forma, o problema passa a ser tratado como um **mTSP (Multiple Traveling Salesman Problem)**.

---

## 🧬 Algoritmo Genético

### Representação

Cada indivíduo é representado por uma permutação de clientes:

```text
[7, 14, 3, 19, 8, 2, 11, 21, ...]
```

A permutação é posteriormente decodificada em **k rotas distintas**, correspondentes aos veículos disponíveis.

### Operadores Evolutivos

#### 🎯 Seleção por Torneio

* Tamanho do torneio: **T = 5**
* Controle equilibrado da pressão seletiva
* Independente da escala dos valores de fitness
* Mantém diversidade populacional

#### 🔄 Crossover

* **Order Crossover (OX)**

Preserva:

* Permutações válidas
* Ordem relativa dos clientes
* Diversidade genética

#### 🧪 Mutação

Operador de troca (swap mutation) aplicado probabilisticamente para introduzir variabilidade genética.

#### 🏆 Elitismo

Os melhores indivíduos são preservados entre gerações para evitar perda de qualidade das soluções.

---

## 📈 Função Objetivo

O fitness corresponde à distância total percorrida por todos os veículos:

```math
Fitness(S) = \sum_{v=1}^{k} Distância(Rota_v)
```

Onde:

* k = número de veículos
* Rotaᵥ = sequência de clientes atribuída ao veículo v

### Objetivo

Minimizar:

```math
Distância Total = Distância(Rota_1) + Distância(Rota_2) + ... + Distância(Rota_k)
```

---

# 📂 Estrutura do Projeto

```text
.
├── main.cpp
├── Makefile
├── all_run_analysis.py
├── graph.py
├── E-n22-k4.evrp
├── E-n76-k7.evrp
│
├── evolution_trace_*.csv
├── stats.*.txt
│
├── graficos_evolucao_E-n22-k4/
├── graficos_evolucao_E-n76-k7/
│
└── README.md
```

---

# 🔧 Pré-requisitos

## Compilação

* GNU Make
* Compilador C++ (g++)

## Análise de Dados

* Python 3

Bibliotecas:

```bash
pip install pandas matplotlib
```

---

# 🚀 Compilação

Na pasta raiz do projeto execute:

```bash
make clean
make
```

Ao final será gerado o executável:

```text
main
```

---

# ▶️ Execução das Instâncias

O algoritmo segue rigorosamente o orçamento computacional da competição:

```text
25.000 avaliações × tamanho do problema
```

Cada experimento é executado automaticamente em:

```text
20 runs independentes
```

permitindo análises estatísticas robustas.

---

## Instância E-n22-k4

### Características

* 22 clientes
* 4 veículos

Execução:

```bash
./main E-n22-k4.evrp
```

Arquivos gerados:

```text
stats.E-n22-k4.evrp.txt

evolution_trace_E-n22-k4.evrp_run_1.csv
...
evolution_trace_E-n22-k4.evrp_run_20.csv
```

---

## Instância E-n76-k7

### Características

* 76 clientes
* 7 veículos

Execução:

```bash
./main E-n76-k7.evrp
```

Arquivos gerados:

```text
stats.E-n76-k7.evrp.txt

evolution_trace_E-n76-k7.evrp_run_1.csv
...
evolution_trace_E-n76-k7.evrp_run_20.csv
```

---

# 📊 Geração dos Gráficos

Após a execução do algoritmo, os arquivos CSV estarão disponíveis para análise.

---

## Dinâmica Evolutiva

Gera gráficos individuais para cada uma das 20 execuções.

### Instância E-n22-k4

```bash
python3 all_run_analysis.py E-n22-k4.evrp
```

### Instância E-n76-k7

```bash
python3 all_run_analysis.py E-n76-k7.evrp
```

Os gráficos serão armazenados automaticamente em:

```text
graficos_evolucao_E-n22-k4/
graficos_evolucao_E-n76-k7/
```

---

## Análise Comparativa e Estabilidade

Execute:

```bash
python3 graph.py
```

Serão gerados:

```text
boxplot_estabilidade.png

comparativo_E-n22-k4.png

comparativo_E-n76-k7.png
```

Esses gráficos permitem:

* Avaliação da estabilidade do AG
* Comparação entre execuções
* Comparação com resultados de referência da competição
* Visualização da qualidade das soluções encontradas

---

# 📚 Fundamentação Teórica

O projeto segue conceitos clássicos de Computação Evolucionista descritos em:

### Eiben & Smith

```text
Introduction to Evolutionary Computing
Springer
```

### Goldberg

```text
Genetic Algorithms in Search,
Optimization and Machine Learning
```

### Bäck

```text
Evolutionary Algorithms in Theory and Practice
```

---

# 🎓 Aplicação Acadêmica

Este projeto foi desenvolvido como suporte experimental para pesquisas em:

* Computação Evolucionista
* Otimização Combinatória
* Problema do Caixeiro Viajante (TSP)
* Multiple Traveling Salesman Problem (mTSP)
* Electric Vehicle Routing Problem (EVRP)
* Metaheurísticas Baseadas em Populações

---

# 👨‍💻 Autor

**Allan Henriques Teixeira Albulquerque Rodrigues**

Projeto desenvolvido para estudos e experimentação em Algoritmos Genéticos aplicados ao roteamento de veículos.
Adaptado do template fornecido pela competição IEEE WCCI 2020 https://mavrovouniotis.github.io/EVRPcompetition2020/
---

## ⭐ Se este projeto foi útil para você

Considere deixar uma estrela no repositório para apoiar futuras pesquisas e melhorias.
