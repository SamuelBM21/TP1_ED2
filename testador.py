import subprocess
import csv
import re

# Caminho do executável
EXECUTAVEL = "./pesquisa"

# Parâmetros dos testes
metodos = [1, 2, 3, 4]
quantidades = [100, 1000, 10000, 100000, 1000000]
situacoes_por_metodo = {
    1: [1],
    2: [3],
    3: [1, 2, 3],
    4: [1, 2, 3]
}

NUM_CHAVES = 10
saida_csv = "resultados_teste.csv"

cabecalho = [
    "Metodo", "Situacao", "Quantidade",
    "Media_Comparacoes", "Media_Transferencias",
    "Media_Tempo_Total", "Media_Tempo_Criacao_Indice", "Media_Tempo_Pesquisa"
]

def gerar_chaves(qtd):
    salto = qtd // (NUM_CHAVES + 1)
    return [(i + 1) * salto for i in range(NUM_CHAVES)]

with open(saida_csv, mode="w", newline="") as arquivo_csv:
    escritor = csv.writer(arquivo_csv)
    escritor.writerow(cabecalho)

    for metodo in metodos:
        situacoes = situacoes_por_metodo[metodo]

        for situacao in situacoes:
            for quantidade in quantidades:
                chaves = gerar_chaves(quantidade)

                comparacoes_list = []
                transferencias_list = []
                tempo_total_list = []
                tempo_criacao_list = []
                tempo_pesquisa_list = []

                for chave in chaves:
                    comando = [
                        EXECUTAVEL, str(metodo), str(quantidade),
                        str(situacao), str(chave)
                    ]

                    print(f"Executando: {' '.join(comando)}")

                    try:
                        resultado = subprocess.run(
                            comando,
                            capture_output=True, text=True, timeout=300
                        )

                        saida = resultado.stdout

                        # Expressões regulares para capturar os valores
                        comparacoes = int(re.search(r"Número de comparações: (\d+)", saida).group(1))
                        transferencias = int(re.search(r"Número de transferências: (\d+)", saida).group(1))
                        tempo_criacao = float(re.search(r"Tempo de criação do índice: ([\d.]+)", saida).group(1))
                        tempo_pesquisa = float(re.search(r"Tempo de pesquisa: ([\d.]+)", saida).group(1))
                        tempo_total = float(re.search(r"Tempo de execução: ([\d.]+)", saida).group(1))

                        comparacoes_list.append(comparacoes)
                        transferencias_list.append(transferencias)
                        tempo_total_list.append(tempo_total)
                        tempo_criacao_list.append(tempo_criacao)
                        tempo_pesquisa_list.append(tempo_pesquisa)

                    except Exception as e:
                        print(f"Erro na execução: {e}")
                        comparacoes_list.append(0)
                        transferencias_list.append(0)
                        tempo_total_list.append(0.0)
                        tempo_criacao_list.append(0.0)
                        tempo_pesquisa_list.append(0.0)

                # Calcula as médias
                media_comp = sum(comparacoes_list) / len(comparacoes_list)
                media_transf = sum(transferencias_list) / len(transferencias_list)
                media_tempo_total = sum(tempo_total_list) / len(tempo_total_list)
                media_tempo_criacao = sum(tempo_criacao_list) / len(tempo_criacao_list)
                media_tempo_pesquisa = sum(tempo_pesquisa_list) / len(tempo_pesquisa_list)

                # Escreve no CSV
                escritor.writerow([
                    metodo, situacao, quantidade,
                    round(media_comp, 2),
                    round(media_transf, 2),
                    f"{media_tempo_total:.6f}",
                    f"{media_tempo_criacao:.6f}",
                    f"{media_tempo_pesquisa:.6f}"
                ])

print(f"\nTestes concluídos. Resultados salvos em {saida_csv}")
