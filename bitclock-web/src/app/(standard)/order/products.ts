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
    id: "bitclock-pcb",
    image: PcbImage,
    name: "Bitclock PCB",
    description:
      "Bitclock PCB with attached E-ink display. Purchase this if you plan to print the enclosure yourself.",
    price: 69,
    priceId: process.env.STRIPE_BITCLOCK_PCB_PRICE,
  },
];

export default products;

export type { ProductInfo };
