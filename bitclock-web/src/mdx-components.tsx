import React from "react";
import "@mantine/code-highlight/styles.css";

import type { MDXComponents } from "mdx/types";
import { CodeHighlight } from "@mantine/code-highlight";
import { Anchor, Title, Code, rem } from "@mantine/core";
import Link from "next/link";

export function useMDXComponents(components: MDXComponents): MDXComponents {
  return {
    h1: (props) => <Title order={1}>{props.children}</Title>,
    h2: (props) => (
      <Title order={2} mt={rem(55)}>
        {props.children}
      </Title>
    ),
    h3: (props) => (
      <Title order={3} mt={rem(45)}>
        {props.children}
      </Title>
    ),
    h4: (props) => (
      <Title order={4} mt={rem(35)}>
        {props.children}
      </Title>
    ),
    h5: (props) => (
      <Title order={5} mt={rem(25)}>
        {props.children}
      </Title>
    ),
    h6: (props) => (
      <Title order={6} mt={10}>
        {props.children}
      </Title>
    ),
    a: (props) => (
      <Anchor component={Link} target="_blank" href={props.href as string}>
        {props.children}
      </Anchor>
    ),
    code: (props) => {
      const lang = props.className?.replace("language-", "");
      if (!props.className) {
        // Inline code block
        return <Code>{props.children}</Code>;
      } else {
        return (
          <CodeHighlight language={lang} code={props.children as string} />
        );
      }
      // return <CodeHighlight language={lang} code={props.children as string} />;
    },
  };
}
