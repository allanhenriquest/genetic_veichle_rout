import os
import matplotlib
# Configura o matplotlib para modo "headless" (não tenta abrir janelas de interface gráfica)
# Isso resolve o erro "Could not load the Qt platform plugin xcb" no WSL/Linux
matplotlib.use('Agg') 
import matplotlib.pyplot as plt
import numpy as np

# ==============================================================================
# CONFIGURAÇÃO DE DADOS
# Valores de referência da competição para comparação (Top 3)
# O professor pediu para comparar o seu melhor resultado com os 3 primeiros.
# Como E-n22 e E-n76 são instâncias clássicas, os top 3 costumam cravar o ótimo global.
# ==============================================================================
COMPETICAO_TOP3 = {
    "E-n22-k4.evrp": {"1º Lugar": 384.67, "2º Lugar": 384.67, "3º Lugar": 384.67},
    "E-n76-k7.evrp": {"1º Lugar": 692.64, "2º Lugar": 692.80, "3º Lugar": 694.50} 
}

def ler_arquivo_stats(nome_arquivo):
    """Lê o arquivo stats.txt gerado pelo C++ e extrai as 20 execuções."""
    if not os.path.exists(nome_arquivo):
        print(f"⚠️  Arquivo '{nome_arquivo}' não encontrado na pasta.")
        return None

    execucoes = []
    try:
        with open(nome_arquivo, 'r') as f:
            linhas = f.readlines()
            
            # As primeiras 20 linhas do arquivo C++ são os valores exatos de cada execução
            for i in range(20):
                # Substitui vírgula por ponto
                valor_limpo = linhas[i].strip().replace(',', '.') 
                execucoes.append(float(valor_limpo))
                
        return execucoes
    except Exception as e:
        print(f"Erro ao ler {nome_arquivo}: {e}")
        return None

def plotar_boxplot(execucoes_n22, execucoes_n76):
    """Gera um Boxplot para mostrar a consistência do algoritmo (Média e Desvio)."""
    fig, ax = plt.subplots(figsize=(8, 6))
    
    dados = []
    labels = []
    
    if execucoes_n22:
        dados.append(execucoes_n22)
        labels.append('E-n22-k4\n(22 Clientes)')
    if execucoes_n76:
        dados.append(execucoes_n76)
        labels.append('E-n76-k7\n(76 Clientes)')
        
    if not dados:
        return
        
    bplot = ax.boxplot(dados, patch_artist=True, labels=labels, 
                       boxprops=dict(facecolor='lightblue', color='blue'),
                       medianprops=dict(color='red', linewidth=2))
                       
    ax.set_title('Distribuição do Fitness nas 20 Execuções (Menor é Melhor)', fontsize=14)
    ax.set_ylabel('Distância Total (Fitness)', fontsize=12)
    ax.grid(True, axis='y', linestyle='--', alpha=0.7)
    
    plt.tight_layout()
    plt.savefig('boxplot_estabilidade.png', dpi=300)
    print("✅ Gráfico salvo: boxplot_estabilidade.png (Use no slide de Resultados)")

def plotar_comparativo_top3(instancia, minhas_execucoes):
    """Gera o gráfico de barras comparando o SEU melhor resultado com o Top 3."""
    if not minhas_execucoes or instancia not in COMPETICAO_TOP3:
        return
        
    meu_melhor_resultado = min(minhas_execucoes)
    
    dados_comp = COMPETICAO_TOP3[instancia]
    labels = ['Meu AG (Melhor)'] + list(dados_comp.keys())
    valores = [meu_melhor_resultado] + list(dados_comp.values())
    
    cores = ['#1f77b4', '#d62728', '#ff7f0e', '#2ca02c']
    
    fig, ax = plt.subplots(figsize=(8, 6))
    barras = ax.bar(labels, valores, color=cores, edgecolor='black')
    
    for barra in barras:
        altura = barra.get_height()
        ax.text(barra.get_x() + barra.get_width()/2., altura + 0.5,
                f'{altura:.2f}', ha='center', va='bottom', fontsize=11, fontweight='bold')
                
    ax.set_title(f'Comparativo com Competidores: {instancia}', fontsize=14)
    ax.set_ylabel('Distância (Fitness) - Quanto menor, melhor', fontsize=12)
    
    ax.set_ylim(bottom=min(valores) * 0.95, top=max(valores) * 1.05)
    
    plt.tight_layout()
    nome_imagem = f'comparativo_{instancia.replace(".evrp", "")}.png'
    plt.savefig(nome_imagem, dpi=300)
    print(f"✅ Gráfico salvo: {nome_imagem} ")

if __name__ == "__main__":
    print("Iniciando geração de gráficos ...\n")
    
    arquivo_n22 = "stats.E-n22-k4.evrp.txt"
    arquivo_n76 = "stats.E-n76-k7.evrp.txt"
    
    runs_n22 = ler_arquivo_stats(arquivo_n22)
    runs_n76 = ler_arquivo_stats(arquivo_n76)
    
    if runs_n22 or runs_n76:
        plotar_boxplot(runs_n22, runs_n76)
    
    if runs_n22:
        plotar_comparativo_top3("E-n22-k4.evrp", runs_n22)
        
    if runs_n76:
        plotar_comparativo_top3("E-n76-k7.evrp", runs_n76)
        
    print("\nProcesso concluído! Copie as imagens '.png' geradas na pasta")