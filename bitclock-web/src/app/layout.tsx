// Import styles of packages that you've installed.
// All packages except `@mantine/hooks` require styles imports
import "@mantine/core/styles.css";
import { Analytics } from "@vercel/analytics/react";

import {
  ColorSchemeScript,
  MantineProvider,
  createTheme,
  Anchor,
  Text,
} from "@mantine/core";
import Link from "@/libs/client-link";
import type { Metadata } from "next";
import { Overpass } from "next/font/google";

const overpass = Overpass({
  subsets: ["latin"],
  variable: "--font-overpass",
  display: "swap",
});

export const metadata: Metadata = {
  title: {
    template: "%s | Bitclock",
    default: "Bitclock",
  },
  description:
    "An open source e-ink desk companion, clock, and air quality monitor.",
  keywords: [
    "bitclock",
    "e-ink",
    "desk",
    "companion",
    "clock",
    "air",
    "quality",
    "monitor",
    "esp32",
  ],
  robots: "index, follow",
};

const theme = createTheme({
  headings: {
    fontFamily: `var(--font-overpass), -apple-system, BlinkMacSystemFont, Segoe UI, Roboto, Helvetica, Arial, sans-serif, Apple Color Emoji, Segoe UI Emoji`,
  },
  primaryColor: "gray",
});

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  let currentDate = new Date();
  return (
    <html lang="en" className={`${overpass.variable}`}>
      <head>
        <ColorSchemeScript />
        <link rel="icon" href="/favicon.ico" sizes="any" />
      </head>
      <body
        style={{
          backgroundColor: "rgb(245, 242, 240)",
          color: "rgb(41, 41, 41)",
        }}
      >
        <MantineProvider theme={theme}>
          {children}
          <Text ta="center" my="xl">
            {/* The company name links to goat-hill.com (same tab, followed) to
                tie Bitclock to its maker for search; it looks like the text
                around it until hovered. */}
            © {currentDate.getFullYear()}{" "}
            <Anchor
              href="https://goat-hill.com/"
              inherit
              c="inherit"
              underline="hover"
            >
              Goat Hill Electronics LLC
            </Anchor>
            . All rights reserved.&nbsp;
            <Anchor component={Link} href="/privacy">
              Privacy Policy
            </Anchor>
            &nbsp;𐤟&nbsp;
            <Anchor component={Link} href="/terms">
              Terms of Service
            </Anchor>
          </Text>
        </MantineProvider>
        <Analytics />
      </body>
    </html>
  );
}
