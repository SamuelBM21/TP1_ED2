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
    2: [1, 2, 3],
    3: [1, 2, 3],
    4: [1, 2, 3]
}

NUM_CHAVES = 10
saida_csv = "resultados_teste.csv"

cabecalho = [
    "Metodo", "Situacao", "Quantidade",
    "Media_Comparacoes_Criacao", "Media_Transferencias_Criacao",
    "Media_Comparacoes_Pesquisa", "Media_Transferencias_Pesquisa",
    "Media_Tempo_Total", "Media_Tempo_Criacao_Indice", "Media_Tempo_Pesquisa"
]

def gerar_chaves(qtd):
    salto = qtd // (NUM_CHAVES + 1)
    return [(i + 1) * salto for i in range(NUM_CHAVES)]

def captura_int(padrao, text, campo, comando):
    m = re.search(padrao, text)
    if not m:
        print(f"[AVISO] não encontrou '{campo}' em: {comando}")
        return 0
    return int(m.group(1))

def captura_float(padrao, text, campo, comando):
    m = re.search(padrao, text)
    if not m:
        print(f"[AVISO] não encontrou '{campo}' em: {comando}")
        return 0.0
    return float(m.group(1))

with open(saida_csv, mode="w", newline="") as arquivo_csv:
    escritor = csv.writer(arquivo_csv)
    escritor.writerow(cabecalho)

    for metodo in metodos:
        situacoes = situacoes_por_metodo[metodo]

        for situacao in situacoes:
            for quantidade in quantidades:
                chaves = gerar_chaves(quantidade)

                comp_criacao_list   = []
                transf_criacao_list = []
                comparacoes_list    = []
                transferencias_list = []
                tempo_total_list    = []
                tempo_criacao_list  = []
                tempo_pesquisa_list = []

                for chave in chaves:
                    comando = [
                        EXECUTAVEL,
                        str(metodo),
                        str(quantidade),
                        str(situacao),
                        str(chave),
                        "-p"
                    ]
                    cmd_str = " ".join(comando)
                    print(f"Executando: {cmd_str}")

                    try:
                        resultado = subprocess.run(
                            comando,
                            capture_output=True, text=True,
                            timeout=240
                        )
                        saida = resultado.stdout

                        # Criação
                        comp_criacao   = captura_int(r"comparações na criação: *(\d+)", saida, "comparações na criação", cmd_str)
                        transf_criacao = captura_int(r"transferências na criação: *(\d+)", saida, "transferências na criação", cmd_str)

                        # Pesquisa
                        comp_pesq      = captura_int(r"Número de comparações: *(\d+)", saida, "Número de comparações", cmd_str)
                        transf_pesq    = captura_int(r"Número de transferências: *(\d+)", saida, "Número de transferências", cmd_str)

                        # Tempos
                        tmp_criacao    = captura_float(r"Tempo de criação do índice: *([\d.]+)", saida, "Tempo de criação do índice", cmd_str)
                        tmp_pesquisa   = captura_float(r"Tempo de pesquisa: *([\d.]+)", saida, "Tempo de pesquisa", cmd_str)
                        tmp_total      = captura_float(r"Tempo de execução: *([\d.]+)", saida, "Tempo de execução", cmd_str)

                        comp_criacao_list.append(comp_criacao)
                        transf_criacao_list.append(transf_criacao)
                        comparacoes_list.append(comp_pesq)
                        transferencias_list.append(transf_pesq)
                        tempo_criacao_list.append(tmp_criacao)
                        tempo_pesquisa_list.append(tmp_pesquisa)
                        tempo_total_list.append(tmp_total)

                    except Exception as e:
                        print(f"[ERRO] execução falhou ({e}), usando zeros")
                        comp_criacao_list.append(0)
                        transf_criacao_list.append(0)
                        comparacoes_list.append(0)
                        transferencias_list.append(0)
                        tempo_criacao_list.append(0.0)
                        tempo_pesquisa_list.append(0.0)
                        tempo_total_list.append(0.0)

                # Médias
                n = len(chaves)
                escritor.writerow([
                    metodo, situacao, quantidade,
                    round(sum(comp_criacao_list)   / n, 2),
                    round(sum(transf_criacao_list) / n, 2),
                    round(sum(comparacoes_list)    / n, 2),
                    round(sum(transferencias_list) / n, 2),
                    f"{(sum(tempo_total_list)    / n):.6f}",
                    f"{(sum(tempo_criacao_list)  / n):.6f}",
                    f"{(sum(tempo_pesquisa_list) / n):.6f}"
                ])

print(f"\nTestes concluídos. Resultados salvos em {saida_csv}")
