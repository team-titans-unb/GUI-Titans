import argparse
import os
from pathlib import Path
from typing import Any, Dict, Callable

PROJECT_CONFIGS: Dict[str, Dict[str, Any]] = {
    "VSSS": {
        "path": "VSSS/Corobeu/configs",
        "var_name": "COR_DO_TIME",
        "formatter": lambda color: f"{1 if color == 'blue' else 0:<28} # 0 -> Amarelo; 1 -> Azul"
    },
    "SSL": {
        "path": "SSL-EL_CBR-2024/Controle",
        "var_name": "TEAM_COLOR",
        "formatter": lambda color: f"'{color}'"
    }
}

def update_config_file(file_path: Path, var_name: str, new_value_formatted: str) -> bool:
    """
    Lê um arquivo de configuração, substitui o valor de uma variável e salva o arquivo.

    Args:
        file_path: O caminho completo para o arquivo 'config.py'.
        var_name: O nome da variável a ser alterada (ex: 'TEAM_COLOR').
        new_value_formatted: A string completa com o novo valor já formatado.

    Returns:
        True se a atualização foi bem-sucedida, False caso contrário.
    """
    if not file_path.is_file():
        print(f"ERRO: Arquivo de configuração não encontrado em '{file_path}'")
        return False

    print(f"Tentando atualizar o arquivo: '{file_path}'...")
    try:
        lines = file_path.read_text().splitlines()
        
        updated_lines = []
        var_found = False
        for line in lines:
            if line.strip().startswith(var_name):
                updated_lines.append(f"{var_name} = {new_value_formatted}")
                var_found = True
            else:
                updated_lines.append(line)
        
        if not var_found:
            updated_lines.append(f"\n{var_name} = {new_value_formatted}")

        file_path.write_text("\n".join(updated_lines) + "\n")
        print(f"SUCESSO: Arquivo '{file_path.name}' atualizado.")
        return True
        
    except IOError as e:
        print(f"ERRO: Falha ao ler ou escrever no arquivo '{file_path}'. Motivo: {e}")
        return False

def main():
    # parser = argparse.ArgumentParser(description="Atualiza a cor do time nos projetos VSSS e SSL.")
    # parser.add_argument(
    #     '--category', 
    #     type=str, 
    #     required=True, 
    #     choices=PROJECT_CONFIGS.keys(),
    #     help="A categoria do projeto."
    # )
    # parser.add_argument(
    #     '--color', 
    #     type=str, 
    #     required=True, 
    #     choices=['blue', 'yellow'],
    #     help="A nova cor do time."
    # )
    # args = parser.parse_args()

    args = argparse.Namespace(category='SSL', color='yellow')  # Simulação de argumentos para teste

    script_dir = Path(__file__).parent
    project_root = (script_dir / '../../').resolve()
    
    config = PROJECT_CONFIGS[args.category]
    project_path = project_root / config["path"]
    config_file = project_path / 'config.py'
    
    formatted_value = config["formatter"](args.color)

    update_config_file(config_file, config["var_name"], formatted_value)

    print("\n--- Atualização de configuração finalizada ---")

if __name__ == "__main__":
    main()