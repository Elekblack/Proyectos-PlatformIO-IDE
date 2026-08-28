#!/usr/bin/env bash
set -euo pipefail

repo_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if command -v pio >/dev/null 2>&1; then
  pio_bin="$(command -v pio)"
elif [[ -x "${HOME}/.platformio/penv/bin/pio" ]]; then
  pio_bin="${HOME}/.platformio/penv/bin/pio"
else
  echo "Error: no se encontró PlatformIO Core (pio)." >&2
  exit 127
fi

projects=(PlayCodeBot_v2 PlayCodeBot Juego_Tetris Juego_Doom ojosAnimados)

for project in "${projects[@]}"; do
  echo "Compilando ${project}..."
  "${pio_bin}" run --project-dir "${repo_dir}/${project}"
done

for project in PlayCodeBot_v2 PlayCodeBot; do
  echo "Ejecutando pruebas nativas de ${project}..."
  "${pio_bin}" test --project-dir "${repo_dir}/${project}" --environment native
done

echo "Todas las compilaciones y pruebas finalizaron correctamente."
