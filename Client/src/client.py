#!/usr/bin/env python3
import sys
import socket
import argparse
import os


def parseCMD():
    parser = argparse.ArgumentParser()
    parser.add_argument('hostname', help='Destination hostname or IP')
    parser.add_argument('port', help='Destination port', type=int)
    parser.add_argument('files', help='Input files to be sent', nargs='+')
    parser.add_argument('-t', '--tcp', action='store_const',
                        dest='tcp', help='Use TCP protocol: Default',
                        required=False, default=True, const=True)
    parser.add_argument('-u', '--udp', action='store_const',
                        dest='udp', help='Use UDP protocol',
                        required=False, default=False, const=True)

    return parser.parse_args()


def main():
    # Process command line arguaments
    args = parseCMD()
    proto = socket.SOCK_STREAM
    if args.udp:
        proto = socket.SOCK_DGRAM

    try:
        sock = socket.socket(socket.AF_INET, proto)
        # Connect to server and send data
        sock.connect((args.hostname, args.port))

        # Send files to server
        for file in args.files:
            try:
                if os.stat(file).st_size > 0:
                    # send contents
                    FHandle = open(file, "r")
                    sock.sendall(bytes(FHandle.read()))
                else:
                    print("Invalid file type, Skipping ({})".format(file))
                    continue
            except OSError:
                print("Unable to open", file)
        sock.close()

    except socket.error as e:
        print(e)

if __name__ == '__main__':
    try:
        main()
    except (KeyboardInterrupt, Exception):
        print("\nProgram exiting...")
