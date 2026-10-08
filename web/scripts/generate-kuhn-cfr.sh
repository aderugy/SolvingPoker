#!/usr/bin/env bash
# Regenerates the precomputed Kuhn CFR and CFR+ runs shipped with the app (src/data/kuhn/{cfr,cfr-plus}/*.json).
# usage (from web/): ./scripts/generate-kuhn-cfr.sh [path/to/kuhn_export]
set -euo pipefail

cd "$(dirname "$0")/.."
BIN="${1:-../cmake-build-debug/kuhn_export}"

cmake --build ../cmake-build-debug --target kuhn_export
for solver in cfr cfr-plus; do
  OUT=src/data/kuhn/$solver
  mkdir -p "$OUT"
  for n in 100 1000 10000 100000; do
    "$BIN" "$n" "$solver" > "$OUT/$n.json"
    echo "wrote $OUT/$n.json"
  done
done
