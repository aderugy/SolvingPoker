import type { Metadata } from "next";
import Link from "next/link";

import { KuhnCfrDemo } from "@/components/kuhn/kuhn-cfr-demo";
import { KUHN_CFR_PLUS_RUNS } from "@/data/kuhn/cfr-plus";

export const metadata: Metadata = {
  title: "Kuhn poker · CFR+",
  description: "CFR+ (regret-matching+) on Kuhn poker.",
};

export default function KuhnCfrPlusPage() {
  return (
    <div className="space-y-8">
      <div className="space-y-2">
        <div className="text-xs font-medium uppercase tracking-wide text-muted">Kuhn poker</div>
        <h1 className="text-2xl font-semibold tracking-tight">CFR+</h1>
        <p className="max-w-2xl text-ink-2">
          Same game as the <Link href="/kuhn/cfr" className="underline">vanilla CFR</Link> demo, solved with CFR+: cumulative
          regrets are clamped at zero after every update, so an action that was bad early on can come back as soon as
          it starts paying off. These runs were precomputed by the C++ solver in{" "}
          <code className="font-mono text-sm">kuhn/</code>, with exploitability measured by a best response against
          the average strategy.
        </p>
      </div>
      <KuhnCfrDemo
        runs={KUHN_CFR_PLUS_RUNS}
        solver="CFR+"
        currentNote="The current (regret-matching+) strategy is what the solver plays at each iteration; only the average comes with a convergence guarantee."
      />
    </div>
  );
}
