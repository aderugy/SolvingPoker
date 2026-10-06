// Registry of solver demos. Each one lives at /<game>/<solver> under src/app.
// Add an entry here when a new solver gets a page.

export interface SolverDemo {
  game: string;
  solver: string;
  href: string;
  title: string;
  description: string;
}

export const SOLVERS: SolverDemo[] = [
  {
    game: "Kuhn poker",
    solver: "Vanilla CFR",
    href: "/kuhn/cfr",
    title: "Kuhn poker · CFR",
    description:
      "Counterfactual regret minimisation on 3-card poker. Watch exploitability fall and the average strategy settle on a Nash equilibrium.",
  },
];
