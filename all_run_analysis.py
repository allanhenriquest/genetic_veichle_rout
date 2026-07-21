import pandas as pd
import matplotlib
# Configuração ESSENCIAL para WSL/Linux: Obriga o matplotlib a não tentar abrir janelas
matplotlib.use('Agg') 
import matplotlib.pyplot as plt
import os
import sys

def plot_run(csv_file, run_id, output_dir):
    """
    Lê o arquivo CSV de uma execução específica e gera o gráfico de dinâmica populacional.
    """
    try:
        # Lê o rastro gerado pelo stats_evolution.cpp
        df = pd.read_csv(csv_file, names=['Generation', 'StdDev', 'Entropy'])
    except Exception as e:
        print(f"  [Erro] Falha ao ler {csv_file}: {e}")
        return False

    if df.empty:
        print(f"  [Aviso] O arquivo {csv_file} está vazio.")
        return False

    fig, ax1 = plt.subplots(figsize=(10, 6))

    # --- Eixo Y Principal (Esquerda): Desvio Padrão do Fitness ---
    color = 'tab:blue'
    ax1.set_xlabel('Gerações')
    ax1.set_ylabel('Desvio Padrão do Fitness (Diversidade)', color=color)
    ax1.plot(df['Generation'], df['StdDev'], color=color, linewidth=1.5, label='Desvio Padrão')
    ax1.tick_params(axis='y', labelcolor=color)

    # --- Eixo Y Secundário (Direita): Entropia Genética ---
    ax2 = ax1.twinx()
    color = 'tab:red'
    ax2.set_ylabel('Entropia Genética (Diversidade Estrutural)', color=color)
    ax2.plot(df['Generation'], df['Entropy'], color=color, linestyle='--', linewidth=1.2, label='Entropia')
    ax2.tick_params(axis='y', labelcolor=color)

    # Título e layout
    plt.title(f'Dinâmica Populacional (Exploração vs Explotação) - Run {run_id}')
    
    # Adiciona uma grade para facilitar a leitura
    ax1.grid(True, linestyle='--', alpha=0.6)
    
    # Ajusta o layout para não cortar rótulos
    fig.tight_layout()
    
    # Salva o gráfico
    output_path = os.path.join(output_dir, f'run_{run_id:02d}_metrics.png')
    plt.savefig(output_path, dpi=200)
    plt.close() # Libera a RAM
    return True

if __name__ == "__main__":
    # Verifica se o usuário passou a instância como argumento
    if len(sys.argv) < 2:
        print("Uso correto: python3 all_run_analysis.py NOME_DA_INSTANCIA.evrp")
        print("Exemplo: python3 all_run_analysis.py E-n22-k4.evrp")
        sys.exit(1)
        
    instancia = sys.argv[1]
    
    # Cria uma pasta limpa para a instância
    output_dir = f'graficos_evolucao_{instancia.replace(".evrp", "")}'
    
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        
    print(f"Gerando gráficos evolutivos para a instância: {instancia}")
    print(f"Os gráficos serão salvos na pasta: '{output_dir}/'")
    
    sucesso = 0
    # Percorre as 20 execuções
    for run in range(1, 21):
        filename = f"evolution_trace_{instancia}_run_{run}.csv"
        if os.path.exists(filename):
            print(f"  Processando {filename}...")
            if plot_run(filename, run, output_dir):
                sucesso += 1
        else:
            print(f"  [Aviso] Arquivo não encontrado: {filename}")
            
    print(f"\n[Concluído] {sucesso} gráficos gerados com sucesso na pasta '{output_dir}/'.")