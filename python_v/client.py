import socket

PORT = 8080
KEY = 0xAA


def xor_cipher(data: str) -> str:
    return "".join(chr(ord(c) ^ KEY) for c in data)


client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect(("127.0.0.1", PORT))

print("Connected to server. Type 'q' to quit.\n")

while True:
    msg = input("Client: ")
    encrypted = xor_cipher(msg)
    client.send(encrypted.encode())

    if msg.lower() == "q":
        print("Client exiting...")
        break

    data = client.recv(1024).decode()
    if not data:
        print("Server closed connection.")
        break

    decrypted = xor_cipher(data)
    if decrypted.lower() == "q":
        print("Server ended the chat.")
        break

    print(f"Server: {decrypted}")

client.close()
