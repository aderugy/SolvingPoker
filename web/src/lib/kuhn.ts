// Shapes produced by kuhn/export.cpp, plus the closed-form Kuhn equilibrium used as a reference.

export type Card = "J" | "Q" | "K";
export type History = "" | "x" | "b" | "xb";

export interface ConvergencePoint {
  iteration: number;
  exploitability: number; // br(OOP) + br(IP)
  brOop: number;
  brIp: number;
}

export interface Infoset {
  key: string; // card + history, e.g. "Kxb"
  card: Card;
  history: History;
  player: 0 | 1; // 0 = OOP, 1 = IP
  average: [number, number]; // [x, b]
  current: [number, number]; // [x, b]
}

export interface CfrRun {
  iterations: number;
  convergence: ConvergencePoint[];
  infosets: Infoset[];
}

export const CARD_ORDER: Card[] = ["J", "Q", "K"];

export const GAME_VALUE_OOP = -1 / 18;

// Decision points in game order, with what action 'x' / 'b' means there.
export const DECISIONS: { history: History; player: 0 | 1; title: string; actions: [string, string] }[] = [
  { history: "", player: 0, title: "OOP opens", actions: ["check", "bet"] },
  { history: "x", player: 1, title: "IP after a check", actions: ["check", "bet"] },
  { history: "b", player: 1, title: "IP facing a bet", actions: ["fold", "call"] },
  { history: "xb", player: 0, title: "OOP facing check–bet", actions: ["fold", "call"] },
];

// Kuhn has a one-parameter family of equilibria: OOP bets J with any alpha in [0, 1/3],
// and every other probability follows from it. Returns P(action 'b') at each infoset.
export function nashBetProbability(card: Card, history: History, alpha: number): number {
  const a = Math.min(Math.max(alpha, 0), 1 / 3);
  const table: Record<History, Record<Card, number>> = {
    "": { J: a, Q: 0, K: 3 * a },
    xb: { J: 0, Q: a + 1 / 3, K: 1 },
    x: { J: 1 / 3, Q: 0, K: 1 },
    b: { J: 0, Q: 1 / 3, K: 1 },
  };
  return table[history][card];
}
