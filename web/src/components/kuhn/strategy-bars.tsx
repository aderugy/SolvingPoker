"use client";

import { useState } from "react";

import { CARD_ORDER, DECISIONS, type Infoset, nashBetProbability } from "@/lib/kuhn";

const pct = (v: number) => `${(v * 100).toFixed(1)}%`;

interface Props {
  infosets: Infoset[];
  mode: "average" | "current";
  alpha: number; // OOP's J bet frequency, picks the Nash reference out of the family
}

export function StrategyBars({ infosets, mode, alpha }: Props) {
  const [hover, setHover] = useState<string | null>(null);
  const byKey = new Map(infosets.map((i) => [i.key, i]));

  return (
    <div className="space-y-6">
      <div className="flex flex-wrap items-center gap-x-5 gap-y-1 text-xs text-ink-2">
        <span className="flex items-center gap-1.5">
          <span className="inline-block h-2.5 w-2.5 rounded-sm bg-check" /> Check / fold
        </span>
        <span className="flex items-center gap-1.5">
          <span className="inline-block h-2.5 w-2.5 rounded-sm bg-bet" /> Bet / call
        </span>
        <span className="flex items-center gap-1.5">
          <span className="inline-block h-3 w-0.5 bg-ink" /> Nash reference (α = {alpha.toFixed(3)})
        </span>
      </div>

      <div className="grid gap-6 sm:grid-cols-2">
        {DECISIONS.map((dec) => (
          <section key={dec.history} className="space-y-2">
            <h3 className="text-sm font-medium">
              {dec.title}
              <span className="ml-2 font-normal text-muted">{dec.actions.join(" / ")}</span>
            </h3>

            {CARD_ORDER.map((card) => {
              const info = byKey.get(card + dec.history);
              if (!info) return null;
              const [x, b] = info[mode];
              const nash = nashBetProbability(card, dec.history, alpha);
              const isAlpha = card === "J" && dec.history === "";
              const active = hover === info.key;

              return (
                <div
                  key={info.key}
                  className="relative grid grid-cols-[1.5rem_1fr_4.5rem] items-center gap-3"
                  onPointerEnter={() => setHover(info.key)}
                  onPointerLeave={() => setHover(null)}
                >
                  <span className="font-mono text-sm font-semibold">{card}</span>

                  <div className="relative h-4">
                    <div className="flex h-full gap-0.5">
                      {x > 0.0005 && <div className="h-full rounded-l bg-check last:rounded-r" style={{ width: `${x * 100}%` }} />}
                      {b > 0.0005 && <div className="h-full rounded-r bg-bet first:rounded-l" style={{ width: `${b * 100}%` }} />}
                    </div>
                    <div
                      className="absolute -top-1 -bottom-1 w-0.5 -translate-x-1/2 rounded-full bg-ink"
                      style={{ left: `${(1 - nash) * 100}%` }}
                    />
                  </div>

                  <span className="text-right text-sm tabular-nums text-ink-2">{pct(b)}</span>

                  {active && (
                    <div className="pointer-events-none absolute left-8 top-6 z-10 w-56 rounded-md border border-hairline bg-surface px-3 py-2 text-xs shadow-sm">
                      <div className="font-medium">
                        {info.key} · {dec.title}
                      </div>
                      <div className="mt-1 grid grid-cols-[auto_auto] gap-x-3 tabular-nums text-ink-2">
                        <span>{dec.actions[0]}</span>
                        <span className="text-right text-ink">{pct(x)}</span>
                        <span>{dec.actions[1]}</span>
                        <span className="text-right text-ink">{pct(b)}</span>
                        <span>Nash {dec.actions[1]}</span>
                        <span className="text-right">{isAlpha ? "free (α)" : pct(nash)}</span>
                      </div>
                    </div>
                  )}
                </div>
              );
            })}
          </section>
        ))}
      </div>
    </div>
  );
}
