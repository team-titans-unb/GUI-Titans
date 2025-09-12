import argparse
from pathlib import Path
from typing import Any, Dict
import json

ROLE_MAP = {
    'goalkeeper': 0,
    'defender': 1,
    'attacker': 2
}

COLOR_MAP = {
    'yellow': 0,
    'blue': 1
}

SIDE_MAP = {
    'left': 0,
    'right': 1
}

COMMON_PROJECT_LOGIC = {
    "config_filename": "config.py",
    "variables": {
        "color": {
            "var_name": "COR_DO_TIME",
            "formatter": lambda color: str(COLOR_MAP.get(color))
        },
        "side": {
            "var_name": "LADO_DO_TIME",
            "formatter": lambda side: str(SIDE_MAP.get(side))
        }
    },
    "robot_logic": {
        "id_var_template": "ID_{name}",
        "role_var_template": "FUNCAO_{name}",
        "role_map": ROLE_MAP
    }
}

PROJECT_CONFIGS: Dict[str, Dict[str, Any]] = {
    "VSSS": {
        "path": "VSSS/Corobeu/configs",
        "valid_robots": ['ZEUS', 'KRATOS', 'ARES'],
        **COMMON_PROJECT_LOGIC
    },
    "SSL": {
        "path": "SSL-EL_CBR-2024/Controle",
        "valid_robots": ['ALVIN', 'SIMON', 'THEODORE'],
        **COMMON_PROJECT_LOGIC
    }
}

def update_variables(file_path: Path, updates: Dict[str, str]) -> bool:
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
                    value_part = f"{var_name} = {new_value}"
                    comment_part = ''
                    if '#' in line:
                        comment_part = line.split('#', 1)[1]
                        updated_lines.append(f"{value_part:<28} #{comment_part}")
                    else:
                        updated_lines.append(value_part)

                    if var_name in vars_to_update: vars_to_update.remove(var_name)
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
    parser = argparse.ArgumentParser(description="Atualiza a configuração do time nos projetos VSSS e SSL.")
    # Argumentos...
    parser.add_argument('--category', type=str, required=True, choices=PROJECT_CONFIGS.keys(), help="VSSS ou SSL.")
    parser.add_argument('--color', type=str, choices=['blue', 'yellow'], help="blue ou yellow.")
    parser.add_argument('--side', type=str, choices=['left', 'right'], help="left ou right.")
    parser.add_argument('--robot-data', type=str, help="String JSON com a lista de configurações dos robôs.")
    args = parser.parse_args()

    # Lógica principal

    # 1. Configuração de caminhos
    project_config = PROJECT_CONFIGS[args.category]
    script_dir = Path(__file__).parent
    project_root = (script_dir / '../../').resolve()
    config_file = project_root / project_config["path"] / project_config["config_filename"]
    print(f"DEBUG SCRIPT: Tentando acessar o arquivo em: {config_file}")
    
    # 2. Criação de UM dicionário para TODAS as atualizações
    updates_to_make = {}

    # 3. Adiciona atualizações globais ao dicionário
    if args.color:
        conf = project_config["variables"]["color"]
        updates_to_make[conf["var_name"]] = conf["formatter"](args.color)
    if args.side:
        conf = project_config["variables"]["side"]
        updates_to_make[conf["var_name"]] = conf["formatter"](args.side)

    # 4. Adiciona atualizações específicas do robô ao dicionário
    if args.robot_data:
        try:
            robot_list = json.loads(args.robot_data)
            robot_logic = project_config['robot_logic']
            valid_names = project_config['valid_robots']

            for robot in robot_list:
                name = robot.get("name", "").upper()
                new_id = robot.get("id")
                new_role = robot.get("role")

                if name not in valid_names:
                    print(f"AVISO: Robô com nome '{name}' inválido para a categoria {args.category}. Ignorando.")
                    continue

                if new_id is not None:
                    var_name = robot_logic['id_var_template'].format(name=name)
                    updates_to_make[var_name] = str(new_id)
                if new_role is not None:
                    var_name = robot_logic['role_var_template'].format(name=name)
                    role_value = robot_logic['role_map'].get(new_role)
                    if role_value is not None:
                        updates_to_make[var_name] = str(role_value)

        except json.JSONDecodeError:
            print("ERRO: Formato de --robot-data inválido. Deve ser uma string JSON.")

    # 4. Aplicação das atualizações

    print("\n--- Iniciando atualização de configuração ---\n")
    if updates_to_make:
        update_variables(config_file, updates_to_make)
    else:
        print("Nenhuma atualização especificada. Nada foi alterado.")

    print("\n--- Atualização de configuração finalizada ---")

if __name__ == "__main__":
    main()