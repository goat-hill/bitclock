import PcbImage from "@/app/img/pcb.jpg";
import BitclockBlackImage from "@/app/img/bitclock-black.jpg";
import BitclockWhiteImage from "@/app/img/bitclock-white.jpg";
import { StaticImageData } from "next/image";

type ProductInfo = {
  id: string;
  name: string;
  image: StaticImageData;
  description: string;
  price: number;
  priceId: string;
};

const products: ProductInfo[] = [
  {
    id: "bitclock-white",
    name: "Bitclock (Rock white)",
    description:
      "Complete Bitclock with a 3D printed rock white PETG enclosure. Rock pattern may vary slightly by print.",
    image: BitclockWhiteImage,
    price: 69,
    priceId: process.env.STRIPE_BITCLOCK_WHITE_PRICE,
  },
  {
    id: "bitclock-black",
    image: BitclockBlackImage,
    name: "Bitclock (Black)",
    description: "Complete Bitclock with a 3D printed black PETG enclosure.",
    price: 69,
    priceId: process.env.STRIPE_BITCLOCK_BLACK_PRICE,
  },
  {
    id: "bitclock-pcb",
    image: PcbImage,
    name: "Bitclock PCB",
    description:
      "Bitclock PCB with attached E-ink display. Purchase this if you plan to print the enclosure yourself.",
    price: 59,
    priceId: process.env.STRIPE_BITCLOCK_PCB_PRICE,
  },
];

export default products;

export type { ProductInfo };
