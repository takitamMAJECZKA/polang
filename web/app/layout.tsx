import type { Metadata } from "next";
import { Geist, Geist_Mono } from "next/font/google";
import "./globals.css";
import Navbar from "./components/Navbar";

const geistSans = Geist({
  variable: "--font-geist-sans",
  subsets: ["latin"],
});

const geistMono = Geist_Mono({
  variable: "--font-geist-mono",
  subsets: ["latin"],
});

export const metadata: Metadata = {
  title: "Benzin - Polski Język Programowania",
  description: "Prosty, interpretowany język programowania z polską składnią.",
};

export default function RootLayout({
  children,
}: Readonly<{
  children: React.ReactNode;
}>) {
  return (
    <html lang="pl">
      <body
        className={`${geistSans.variable} ${geistMono.variable} antialiased bg-[#1e1e1e] text-[#d4d4d4] h-screen flex flex-col overflow-hidden`}
      >
        <Navbar />
        <main className="flex-1 overflow-hidden flex flex-col">
          {children}
        </main>
      </body>
    </html>
  );
}
