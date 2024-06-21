import Stripe from "stripe";
import products from "@/app/(standard)/order/products";
import { NextRequest } from "next/server";

const stripe = new Stripe(process.env.STRIPE_SECRET_KEY);

type ApiError = {
  error: string;
};

type CheckoutSessionGetResponse =
  | {
      status?: string;
      customer_email?: string;
    }
  | ApiError;

type CheckoutSessionPostResponse =
  | {
      clientSecret?: string;
    }
  | ApiError;

async function getImpl(
  req: NextRequest,
): Promise<[CheckoutSessionGetResponse, ResponseInit]> {
  try {
    const { searchParams } = new URL(req.url);
    const sessionId = searchParams.get("session_id");

    if (!sessionId) {
      return [{ error: "Missing session_id" }, { status: 400 }];
    }

    const session = await stripe.checkout.sessions.retrieve(sessionId);

    return [
      {
        status: session.status ?? undefined,
        customer_email: session.customer_details?.email ?? undefined,
      },
      { status: 200 },
    ];
  } catch (err) {
    if (err instanceof stripe.errors.StripeError) {
      return [{ error: err.message }, { status: err.statusCode || 500 }];
    } else {
      return [{ error: `Unexpected error: ${err}` }, { status: 500 }];
    }
  }
}

export async function GET(req: NextRequest): Promise<Response> {
  const [data, responseInit] = await getImpl(req);
  return Response.json(data, responseInit);
}

async function postImpl(
  req: NextRequest,
): Promise<[CheckoutSessionPostResponse, ResponseInit]> {
  const { productId } = await req.json();
  const selectedProduct = products.find((product) => product.id == productId);

  if (!selectedProduct) {
    return [{ error: "Unrecognized product ID" }, { status: 400 }];
  }

  try {
    // Create Checkout Sessions from body params.
    const session = await stripe.checkout.sessions.create({
      ui_mode: "embedded",
      line_items: [
        {
          price: selectedProduct.priceId,
          quantity: 1,
          dynamic_tax_rates: [process.env.STRIPE_CA_TAX_RATE],
          adjustable_quantity: {
            enabled: true,
            maximum: 50,
          },
        },
      ],
      shipping_address_collection: {
        allowed_countries: ["US"],
      },
      shipping_options: [
        { shipping_rate: process.env.STRIPE_BITCLOCK_SHIPPING_ID },
      ],
      mode: "payment",
      return_url: `${process.env.STRIPE_CALLBACK_SCHEME}${process.env.NEXT_PUBLIC_VERCEL_URL}/order_cb?session_id={CHECKOUT_SESSION_ID}`,
    });

    return [
      { clientSecret: session.client_secret ?? undefined },
      { status: 200 },
    ];
  } catch (err) {
    if (err instanceof stripe.errors.StripeError) {
      return [{ error: err.message }, { status: err.statusCode || 500 }];
    } else {
      return [{ error: `Unexpected error: ${err}` }, { status: 500 }];
    }
  }
}

export async function POST(req: NextRequest): Promise<Response> {
  const [data, responseInit] = await postImpl(req);
  return Response.json(data, responseInit);
}

export type { CheckoutSessionGetResponse, CheckoutSessionPostResponse };
