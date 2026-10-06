# Solving Poker — web demos

Next.js (TypeScript, Tailwind) front end for the C++ solvers in the parent repo.
Each solver gets its own route at `/<game>/<solver>` and an entry in `src/lib/solvers.ts`.

| Route | Solver | Backed by |
|---|---|---|
| `/kuhn/cfr` | Vanilla CFR on Kuhn poker | precomputed runs in `src/data/kuhn/cfr/*.json` |

The app is fully static: solver output is generated offline and bundled, so nothing runs server-side.

## Run

```bash
pnpm install
pnpm dev
```

## Regenerating solver data

After changing the Kuhn solver, rebuild its exporter (`kuhn/export.cpp`, CMake target `kuhn_export`)
and rewrite the bundled runs:

```bash
./scripts/generate-kuhn-cfr.sh            # uses ../cmake-build-debug/kuhn_export
./scripts/generate-kuhn-cfr.sh path/to/kuhn_export
```
