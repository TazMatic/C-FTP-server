#!/usr/bin/env python3
import sys
import socket


def main():
    # Process command line arguaments
    numberOfFiles = len(sys.argv)
    helpString = "client.py host port protocal <...Files>"
    if numberOfFiles < 5:
        print("Error not enough arguments:", helpString)
        return
    # Store connection variables
    host = sys.argv[1]
    port = sys.argv[2]
    proto = sys.argv[3]
    numberOfFiles -= 4

    # Establish Connection
    if proto.lower() == "udp":
        proto = socket.SOCK_DGRAM
    elif proto.lower() == "tcp":
        proto = socket.SOCK_STREAM
    else:
        print("Invalid protocol: Please select UPD or TCP")
        return
    print("Made it")
if __name__ == '__main__':
    try:
        main()
    except (KeyboardInterrupt, Exception):
        print("\nProgram exiting...")
