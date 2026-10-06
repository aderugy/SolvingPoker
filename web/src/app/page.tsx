import Link from "next/link";

import { SOLVERS } from "@/lib/solvers";

export default function Home() {
  return (
    <div className="space-y-6">
      <div className="space-y-1">
        <h1 className="text-2xl font-semibold tracking-tight">Solvers</h1>
        <p className="text-ink-2">Pick a solver to run it and inspect the strategy it converges to.</p>
      </div>

      <ul className="grid gap-3 sm:grid-cols-2">
        {SOLVERS.map((s) => (
          <li key={s.href}>
            <Link
              href={s.href}
              className="block h-full rounded-lg border border-hairline bg-surface p-4 transition-colors hover:border-axis"
            >
              <div className="text-xs font-medium uppercase tracking-wide text-muted">{s.game}</div>
              <div className="mt-1 font-semibold">{s.solver}</div>
              <p className="mt-2 text-sm text-ink-2">{s.description}</p>
            </Link>
          </li>
        ))}
      </ul>
    </div>
  );
}
