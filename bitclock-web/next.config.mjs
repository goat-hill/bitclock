/** @type {import('next').NextConfig} */
const nextConfig = {
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

export default nextConfig;
