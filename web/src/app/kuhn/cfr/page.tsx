import type { Metadata } from "next";

import { KuhnCfrDemo } from "@/components/kuhn/kuhn-cfr-demo";

export const metadata: Metadata = {
  title: "Kuhn poker · CFR",
  description: "Vanilla counterfactual regret minimisation on Kuhn poker.",
};

export default function KuhnCfrPage() {
  return (
    <div className="space-y-8">
      <div className="space-y-2">
        <div className="text-xs font-medium uppercase tracking-wide text-muted">Kuhn poker</div>
        <h1 className="text-2xl font-semibold tracking-tight">Vanilla CFR</h1>
        <p className="max-w-2xl text-ink-2">
          Three cards (J, Q, K), one card each, one bet size. OOP acts first. These runs were precomputed by the C++
          solver in <code className="font-mono text-sm">kuhn/</code>, with exploitability measured by a best response
          against the average strategy.
        </p>
      </div>
      <KuhnCfrDemo />
    </div>
  );
}
