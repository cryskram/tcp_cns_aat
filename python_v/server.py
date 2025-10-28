import socket

PORT = 8080
KEY = 0xAA


def xor_cipher(data: str) -> str:
    return "".join(chr(ord(c) ^ KEY) for c in data)


server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(("127.0.0.1", PORT))
server.listen(3)

print(f"Server listening on port {PORT}...")

while True:
    conn, addr = server.accept()
    print(f"Connected by {addr}")

    while True:
        data = conn.recv(1024).decode()
        if not data:
            print("Client disconnected.")
            break

        msg = xor_cipher(data)
        if msg.lower() == "q":
            print("Client requested termination.")
            conn.close()
            break

        print(f"Client: {msg}")
        reply = input("Server: ")
        if reply.lower() == "q":
            conn.send(xor_cipher(reply).encode())
            print("Server exiting chat.")
            conn.close()
            break

        conn.send(xor_cipher(reply).encode())
