import uuid

def generate_reversed_uuid_hex():
    # Generate a 128-bit UUID
    generated_uuid = uuid.uuid4()

    # Get the bytes of the UUID
    uuid_bytes = generated_uuid.bytes

    # Reverse the byte order
    reversed_bytes = uuid_bytes[::-1]

    # Convert the bytes to hex and join them with commas
    hex_bytes = ', '.join(f'0x{byte:02x}' for byte in reversed_bytes)
    print(generated_uuid)
    print(hex_bytes)

    return hex_bytes

generate_reversed_uuid_hex()
