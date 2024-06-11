function stringToArrayBuffer(str: string): ArrayBuffer {
  const encoder = new TextEncoder(); // TextEncoder defaults to UTF-8
  return encoder.encode(str).buffer;
}

export { stringToArrayBuffer };
