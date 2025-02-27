//  Hello World server
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int main (void)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();     //  Socket to talk to clients
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);   

    while (1) {
        char buffer [20];   
        int bytes_received = zmq_recv (responder, buffer, 19, 0);
        buffer[bytes_received] = '\0';
        // Print received message
        printf("Received: %s\n", buffer);
        sleep (1);          //  Do some 'work'
        if(strcmp(buffer, "0001") == 0){
            zmq_send (responder, "Time is 14:44", 13, 0);
        }
        else if(strcmp(buffer, "0002") == 0){
            zmq_send (responder, "Going forward", 13, 0);
        }
        else{
            zmq_send (responder, "Unrecognized command", 20, 0);
        }
    }
    return 0;
}