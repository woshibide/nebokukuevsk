import socket
import time

UDP_IP = "COMPUTER_IP" 
UDP_PORT = 12000 # of listens here

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    message = "hello from pi"
    sock.sendto(message.encode(), (UDP_IP, UDP_PORT))
    time.sleep(1)
