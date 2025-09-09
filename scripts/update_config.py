import argparse
from pathlib import Path
from typing import Any, Dict

PROJECT_CONFIGS: Dict[str, Dict[str, Any]] = {
    "VSSS": {
        "path": "VSSS/Corobeu/configs",
        "variables": {
            "color": {
                "var_name": "COR_DO_TIME",
                "formatter": lambda value: f"{1 if value == 'blue' else 0:<29} # 0 -> Amarelo; 1 -> Azul"
            },
            "side": {
                "var_name": "LADO_DO_TIME",
                "formatter": lambda value: f"{1 if value == 'right' else 0:<28} # 0 -> Esquerdo; 1 -> Direito"
            }
        }
    },
    "SSL": {
        "path": "SSL-EL_CBR-2024/Controle",
        "variables": {
            "color": {
                "var_name": "TEAM_COLOR",
                "formatter": lambda value: f"'{value}'"
            },
            "side": {
                "var_name": "SIDE_TEAM",
                "formatter": lambda value: f"'{value}'"
            }
        }
    }
}

def update_config_file(file_path: Path, updates: Dict[str, str]) -> bool:
    """
    Lê um arquivo de configuração, aplica múltiplas atualizações de variáveis e salva o arquivo uma única vez.

    Args:
        file_path: O caminho completo para o arquivo 'config.py'.
        updates: Um dicionário contendo os nomes das variáveis a serem alteradas e seus novos valores.
    """
    if not file_path.is_file():
        print(f"ERRO: Arquivo de configuração não encontrado em '{file_path}'")
        return False

    print(f"Atualizando {len(updates)} variável(is) em '{file_path.name}'...")
    try:
        lines = file_path.read_text().splitlines()
        updated_lines = []
        vars_to_update = set(updates.keys())

        for line in lines:
            match_found = False
            for var_name, new_value in updates.items():
                if line.strip().startswith(var_name):
                    updated_lines.append(f"{var_name} = {new_value}")
                    vars_to_update.remove(var_name)
                    match_found = True
                    break

            if not match_found:
                updated_lines.append(line)

        for var_name in vars_to_update:
            updated_lines.append(f"\n{var_name} = {updates[var_name]}")

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
    #     choices=['blue', 'yellow'],
    #     help="A nova cor do time."
    # )
    # parser.add_argument(
    #     '--side',
    #     type=str,
    #     choices=['left', 'right'],
    #     help="O novo lado do campo (left/right)."
    # )
    # args = parser.parse_args()

    args = argparse.Namespace(category='SSL', color='blue', side=None)  # Simulação de argumentos para teste

    if args.category not in PROJECT_CONFIGS:
        print(f"ERRO: Categoria inválida '{args.category}'. Opções válidas são: {list(PROJECT_CONFIGS.keys())}")
        return
    if not args.color and not args.side:
        print("ERRO: Especifique ao menos uma variável para atualizar (--color ou --side).")
        return

    script_dir = Path(__file__).parent
    project_root = (script_dir / '../../').resolve()
    
    project_config = PROJECT_CONFIGS[args.category]
    config_file = project_root / project_config["path"] / 'config.py'

    updates_to_apply = {}

    if args.color:
        color_config = project_config["variables"]["color"]
        updates_to_apply[color_config["var_name"]] = color_config["formatter"](args.color)

    if args.side:
        side_config = project_config["variables"]["side"]
        updates_to_apply[side_config["var_name"]] = side_config["formatter"](args.side)

    success = update_config_file(config_file, updates_to_apply)
    if not success:
        print("Falha ao atualizar a configuração.")
        return
    print("\n--- Atualização de configuração finalizada ---")

if __name__ == "__main__":
    main()