#!/usr/bin/env bash
# Regenerates the precomputed Kuhn CFR runs shipped with the app (src/data/kuhn/cfr/*.json).
# usage (from web/): ./scripts/generate-kuhn-cfr.sh [path/to/kuhn_export]
set -euo pipefail

cd "$(dirname "$0")/.."
BIN="${1:-../cmake-build-debug/kuhn_export}"
OUT=src/data/kuhn/cfr

cmake --build ../cmake-build-debug --target kuhn_export
mkdir -p "$OUT"
for n in 100 1000 10000 100000; do
  "$BIN" "$n" > "$OUT/$n.json"
  echo "wrote $OUT/$n.json"
done
