"use client";

import { useEffect, useRef, useState } from "react";

import type { ConvergencePoint } from "@/lib/kuhn";

const HEIGHT = 260;
const M = { top: 12, right: 16, bottom: 32, left: 52 };
const FLOOR = 1e-12; // log scale: clamp exact zeros

function powersOf10(lo: number, hi: number): number[] {
  const out: number[] = [];
  for (let e = Math.floor(Math.log10(lo)); e <= Math.ceil(Math.log10(hi)); e++)
    out.push(10 ** e);
  return out;
}

function formatPow(v: number): string {
  const e = Math.round(Math.log10(v));
  if (e >= 0 && e <= 5) return v.toLocaleString("en-US");
  return `1e${e}`;
}

export function fmt(v: number, digits = 4): string {
  return Math.abs(v) < 1e-3 && v !== 0 ? v.toExponential(2) : v.toFixed(digits);
}

export function ConvergenceChart({ points }: { points: ConvergencePoint[] }) {
  const wrapRef = useRef<HTMLDivElement>(null);
  const [width, setWidth] = useState(0); // 0 until the container is measured
  const [hover, setHover] = useState<number | null>(null);

  useEffect(() => {
    const el = wrapRef.current;
    if (!el) return;
    const ro = new ResizeObserver(([entry]) =>
      setWidth(entry.contentRect.width),
    );
    ro.observe(el);
    return () => ro.disconnect();
  }, []);

  const ys = points.map((p) => Math.max(p.exploitability, FLOOR));
  const xMax = Math.max(points[points.length - 1]?.iteration ?? 10, 10);
  const yTicks = powersOf10(Math.min(...ys), Math.max(...ys));
  const [yLo, yHi] = [yTicks[0], yTicks[yTicks.length - 1]];
  const xTicks = powersOf10(1, xMax).filter((t) => t <= xMax);

  const innerW = Math.max(width - M.left - M.right, 10);
  const innerH = HEIGHT - M.top - M.bottom;
  const sx = (it: number) =>
    M.left + (Math.log10(it) / Math.log10(xMax)) * innerW;
  const sy = (v: number) =>
    M.top +
    (1 -
      (Math.log10(Math.max(v, FLOOR)) - Math.log10(yLo)) /
        (Math.log10(yHi) - Math.log10(yLo) || 1)) *
      innerH;

  const d = points
    .map(
      (p, i) =>
        `${i ? "L" : "M"}${sx(p.iteration).toFixed(1)},${sy(p.exploitability).toFixed(1)}`,
    )
    .join("");

  const onMove = (e: React.PointerEvent<SVGRectElement>) => {
    const rect = e.currentTarget.getBoundingClientRect();
    const x = e.clientX - rect.left + M.left;
    let best = 0;
    for (let i = 1; i < points.length; i++)
      if (
        Math.abs(sx(points[i].iteration) - x) <
        Math.abs(sx(points[best].iteration) - x)
      )
        best = i;
    setHover(best);
  };

  const hp = hover !== null ? points[hover] : null;
  const last = points[points.length - 1];

  return (
    <div
      ref={wrapRef}
      className="relative w-full min-w-0 overflow-hidden"
      style={{ height: HEIGHT }}
    >
      {width > 0 && (
        <svg
          width={width}
          height={HEIGHT}
          role="img"
          aria-label="Exploitability against iterations, log–log"
        >
          {yTicks.map((t) => (
            <g key={`y${t}`}>
              <line
                x1={M.left}
                x2={M.left + innerW}
                y1={sy(t)}
                y2={sy(t)}
                stroke="var(--grid)"
                strokeWidth={1}
              />
              <text
                x={M.left - 8}
                y={sy(t)}
                dy="0.32em"
                textAnchor="end"
                className="fill-muted text-[11px] tabular-nums"
              >
                {formatPow(t)}
              </text>
            </g>
          ))}
          {xTicks.map((t) => (
            <text
              key={`x${t}`}
              x={sx(t)}
              y={HEIGHT - 10}
              textAnchor="middle"
              className="fill-muted text-[11px] tabular-nums"
            >
              {formatPow(t)}
            </text>
          ))}
          <line
            x1={M.left}
            x2={M.left + innerW}
            y1={M.top + innerH}
            y2={M.top + innerH}
            stroke="var(--axis)"
          />

          <path
            d={d}
            fill="none"
            stroke="var(--series-bet)"
            strokeWidth={2}
            strokeLinejoin="round"
            strokeLinecap="round"
          />

          {last && (
            <circle
              cx={sx(last.iteration)}
              cy={sy(last.exploitability)}
              r={4}
              fill="var(--series-bet)"
              stroke="var(--surface)"
              strokeWidth={2}
            />
          )}

          {hp && (
            <g pointerEvents="none">
              <line
                x1={sx(hp.iteration)}
                x2={sx(hp.iteration)}
                y1={M.top}
                y2={M.top + innerH}
                stroke="var(--axis)"
              />
              <circle
                cx={sx(hp.iteration)}
                cy={sy(hp.exploitability)}
                r={5}
                fill="var(--series-bet)"
                stroke="var(--surface)"
                strokeWidth={2}
              />
            </g>
          )}

          <rect
            x={M.left}
            y={M.top}
            width={innerW}
            height={innerH}
            fill="transparent"
            onPointerMove={onMove}
            onPointerLeave={() => setHover(null)}
          />
        </svg>
      )}

      {hp && (
        <div
          className="pointer-events-none absolute z-10 rounded-md border border-hairline bg-surface px-3 py-2 text-xs shadow-sm"
          style={{
            top: M.top,
            left: Math.min(sx(hp.iteration) + 12, width - 180),
          }}
        >
          <div className="font-medium">
            Iteration {hp.iteration.toLocaleString("en-US")}
          </div>
          <div className="mt-1 grid grid-cols-[auto_auto] gap-x-3 tabular-nums text-ink-2">
            <span>Exploitability</span>
            <span className="text-right text-ink">
              {fmt(hp.exploitability, 5)}
            </span>
            <span>BR value, OOP</span>
            <span className="text-right">{fmt(hp.brOop, 5)}</span>
            <span>BR value, IP</span>
            <span className="text-right">{fmt(hp.brIp, 5)}</span>
          </div>
        </div>
      )}
    </div>
  );
}
