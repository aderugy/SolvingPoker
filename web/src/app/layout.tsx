import type { Metadata } from "next";
import Link from "next/link";
import "./globals.css";

export const metadata: Metadata = {
  title: { default: "Solving Poker", template: "%s · Solving Poker" },
  description: "Interactive demos of poker solvers.",
};

export default function RootLayout({ children }: LayoutProps<"/">) {
  return (
    <html lang="en" className="h-full antialiased">
      <body className="min-h-full flex flex-col font-sans">
        <header className="border-b border-hairline">
          <div className="mx-auto max-w-5xl px-4 py-3">
            <Link href="/" className="text-sm font-semibold tracking-tight">
              Solving Poker
            </Link>
          </div>
        </header>
        <main className="mx-auto w-full max-w-5xl flex-1 px-4 py-8">{children}</main>
      </body>
    </html>
  );
}
