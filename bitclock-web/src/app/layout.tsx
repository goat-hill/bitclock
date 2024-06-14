// Import styles of packages that you've installed.
// All packages except `@mantine/hooks` require styles imports
import "@mantine/core/styles.css";
import { Analytics } from "@vercel/analytics/react";

import {
  Center,
  ColorSchemeScript,
  MantineProvider,
  createTheme,
  Box,
  Anchor,
  Text,
} from "@mantine/core";
import Link from "next/link";
import { Overpass } from "next/font/google";

const overpass = Overpass({
  subsets: ["latin"],
  variable: "--font-overpass",
  display: "swap",
});

export const metadata = {
  title: "Bitclock",
  description: "Desk companion",
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
            © {currentDate.getFullYear()} Goat Hill Electronics LLC. All rights
            reserved.&nbsp;
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
