"use client";

import { useState } from "react";

import { ConvergenceChart, fmt } from "@/components/kuhn/convergence-chart";
import { StrategyBars } from "@/components/kuhn/strategy-bars";
import { KUHN_CFR_RUNS } from "@/data/kuhn/cfr";
import { GAME_VALUE_OOP } from "@/lib/kuhn";

export function KuhnCfrDemo() {
  const [selected, setSelected] = useState(1);
  const [mode, setMode] = useState<"average" | "current">("average");

  const run = KUHN_CFR_RUNS[selected];
  const last = run.convergence[run.convergence.length - 1];
  const alpha = run.infosets.find((i) => i.key === "J")?.average[1] ?? 0;

  return (
    <div className="space-y-8">
      <fieldset className="space-y-1">
        <legend className="text-xs font-medium text-ink-2">Iterations</legend>
        <div className="flex flex-wrap gap-1">
          {KUHN_CFR_RUNS.map((r, i) => (
            <button
              key={r.iterations}
              type="button"
              onClick={() => setSelected(i)}
              aria-pressed={selected === i}
              className={`rounded-md border px-3 py-1.5 text-sm tabular-nums transition-colors ${
                selected === i
                  ? "border-ink bg-ink text-page"
                  : "border-hairline bg-surface hover:border-axis"
              }`}
            >
              {r.iterations.toLocaleString("en-US")}
            </button>
          ))}
        </div>
      </fieldset>

      <section className="grid gap-3 sm:grid-cols-3">
        <Stat
          label="Exploitability"
          value={fmt(last.exploitability, 5)}
          note={`after ${run.iterations.toLocaleString("en-US")} iterations`}
          hero
        />
        <Stat
          label="Best response vs OOP"
          value={fmt(last.brIp, 4)}
          note={`Nash value ${fmt(-GAME_VALUE_OOP, 4)}`}
        />
        <Stat
          label="Best response vs IP"
          value={fmt(last.brOop, 4)}
          note={`Nash value ${fmt(GAME_VALUE_OOP, 4)}`}
        />
      </section>

      <section className="space-y-3 rounded-lg border border-hairline bg-surface p-4">
        <div>
          <h2 className="font-semibold">Exploitability over iterations</h2>
          <p className="text-sm text-ink-2">
            br(OOP) + br(IP) against the average strategy, both axes
            logarithmic. Zero at an exact equilibrium.
          </p>
        </div>
        <ConvergenceChart points={run.convergence} />
        <details className="text-sm">
          <summary className="cursor-pointer text-ink-2">Show as table</summary>
          <div className="mt-2 max-h-64 overflow-auto">
            <table className="w-full text-right tabular-nums">
              <thead className="sticky top-0 bg-surface text-xs text-muted">
                <tr>
                  <th className="py-1 font-medium">Iteration</th>
                  <th className="py-1 font-medium">Exploitability</th>
                  <th className="py-1 font-medium">BR OOP</th>
                  <th className="py-1 font-medium">BR IP</th>
                </tr>
              </thead>
              <tbody className="text-ink-2">
                {run.convergence.map((p) => (
                  <tr key={p.iteration} className="border-t border-hairline">
                    <td className="py-1">
                      {p.iteration.toLocaleString("en-US")}
                    </td>
                    <td className="py-1">{fmt(p.exploitability, 6)}</td>
                    <td className="py-1">{fmt(p.brOop, 6)}</td>
                    <td className="py-1">{fmt(p.brIp, 6)}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </details>
      </section>

      <section className="space-y-4 rounded-lg border border-hairline bg-surface p-4">
        <div className="flex flex-wrap items-start justify-between gap-3">
          <div>
            <h2 className="font-semibold">Strategy by infoset</h2>
            <p className="text-sm text-ink-2">
              {mode === "average"
                ? "The average strategy is what CFR guarantees converges to equilibrium."
                : "The current (regret-matching) strategy keeps oscillating; it is not what converges."}
            </p>
          </div>
          <div
            className="flex rounded-md border border-hairline p-0.5 text-sm"
            role="radiogroup"
            aria-label="Strategy shown"
          >
            {(["average", "current"] as const).map((m) => (
              <button
                key={m}
                type="button"
                role="radio"
                aria-checked={mode === m}
                onClick={() => setMode(m)}
                className={`rounded px-3 py-1 capitalize ${mode === m ? "bg-ink text-page" : "text-ink-2"}`}
              >
                {m}
              </button>
            ))}
          </div>
        </div>
        <StrategyBars infosets={run.infosets} mode={mode} alpha={alpha} />
      </section>
    </div>
  );
}

function Stat({
  label,
  value,
  note,
  hero,
}: {
  label: string;
  value: string;
  note: string;
  hero?: boolean;
}) {
  return (
    <div className="rounded-lg border border-hairline bg-surface p-4">
      <div className="text-sm text-ink-2">{label}</div>
      <div className={`mt-1 font-semibold ${hero ? "text-3xl" : "text-2xl"}`}>
        {value}
      </div>
      <div className="mt-1 text-xs text-muted">{note}</div>
    </div>
  );
}
