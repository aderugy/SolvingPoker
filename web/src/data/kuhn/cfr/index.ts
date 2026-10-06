// Precomputed vanilla CFR runs from kuhn/export.cpp.
// Regenerate with web/scripts/generate-kuhn-cfr.sh after changing the solver.

import type { CfrRun } from "@/lib/kuhn";

import run100 from "./100.json";
import run1000 from "./1000.json";
import run10000 from "./10000.json";
import run100000 from "./100000.json";

export const KUHN_CFR_RUNS = [run100, run1000, run10000, run100000] as CfrRun[];
