#
#   Hello World client in Python
#   Connects REQ socket to tcp://localhost:5555
#   Sends "Hello" to server, expects "World" back
#

import zmq
import time

context = zmq.Context()

# Set PicoPi's IP address here
PICO_IP = "192.168.0.121"  # Change this to PicoPi's actual IP
PORT = 5555

context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect(f"tcp://{PICO_IP}:{PORT}")  # Connect to PicoPi server


# #  Socket to talk to server
# print("Connecting to hello world server…")
# socket = context.socket(zmq.REQ)
# socket.connect("tcp://localhost:5555")
print("Conencted to server")

#  Do 10 requests, waiting each time for a response
# for request in range(10):
#     print(f"Sending request {request} …")
#     socket.send(b"This is working")

#     #  Get the reply.
#     message = socket.recv()
#     print(f"Received reply {request} [ {message} ]")

while(1):
    socket.send(b"0001")
    #  Get the reply.
    message = socket.recv()
    print(f"Received reply[ {message} ]")
    time.sleep(1)


    socket.send(b"0002")
    #  Get the reply.
    message = socket.recv()
    print(f"Received reply[ {message} ]")
    time.sleep(1)