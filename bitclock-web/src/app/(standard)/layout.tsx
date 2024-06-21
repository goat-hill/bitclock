import React, { PropsWithChildren } from "react";

import { Container, Box } from "@mantine/core";
import Logo from "@/app/logo";

const RootLayout: React.FC<PropsWithChildren> = ({ children }) => {
  return (
    <Container py="md" size="lg" style={{ lineHeight: 1.8 }}>
      <Box style={{ width: 200, marginLeft: "auto", marginRight: "auto" }}>
        <Logo />
      </Box>
      {children}
    </Container>
  );
};

export default RootLayout;
