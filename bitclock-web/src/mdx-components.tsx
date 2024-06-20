import React from "react";
import "@mantine/code-highlight/styles.css";

import type { MDXComponents } from "mdx/types";
import { CodeHighlight } from "@mantine/code-highlight";
// This file is required to use MDX in `app` directory.
export function useMDXComponents(components: MDXComponents): MDXComponents {
  return {
    // Allows customizing built-in components, e.g. to add styling.
    // h1: ({ children }) => <h1 style={{ fontSize: "100px" }}>{children}</h1>,
    code: (props) => {
      const lang = props.className?.replace("language-", "");
      return <CodeHighlight language={lang} code={props.children as string} />;
    },
  };
}
