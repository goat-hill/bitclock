import withMDX from "@next/mdx";

/** @type {import('next').NextConfig} */
const nextConfig = {
  pageExtensions: ["ts", "tsx", "js", "jsx", "md", "mdx"],
  async redirects() {
    return [
      {
        source: "/c",
        destination: "/connect",
        permanent: false,
      },
    ];
  },
};

export default withMDX()(nextConfig);
