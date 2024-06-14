import { Box, Center } from "@mantine/core";
import Link from "next/link";
import React from "react";

export default function Logo() {
  return (
    <Center>
      <Box w={{ base: 150, md: 400, xl: 600 }}>
        <Link href="/" style={{ color: "rgb(41, 41, 41)" }}>
          <svg
            xmlns="http://www.w3.org/2000/svg"
            viewBox="0 0 29 7"
            shapeRendering="crispEdges"
          >
            <path
              stroke="currentColor"
              d="M0 0h1M15 0h1M26 0h1M0 1h1M5 1h1M8 1h1M15 1h1M26 1h1M0 2h1M8 2h1M15 2h1M26 2h1M0 3h4M5 3h1M7 3h3M11 3h3M15 3h1M17 3h4M22 3h3M26 3h1M28 3h1M0 4h1M3 4h1M5 4h1M8 4h1M11 4h1M15 4h1M17 4h1M20 4h1M22 4h1M26 4h2M0 5h1M3 5h1M5 5h1M8 5h1M11 5h1M15 5h1M17 5h1M20 5h1M22 5h1M26 5h2M0 6h4M5 6h1M8 6h2M11 6h3M15 6h1M17 6h4M22 6h3M26 6h1M28 6h1"
            />
          </svg>
        </Link>
      </Box>
    </Center>
  );
}
