#!/usr/bin/env python3
import socket
import argparse
import os


class package:
    def __init__(self, type, content):
        self.type = type
        self.content = content

    def send(self, sock):
        try:
            sock.send(bytes(str(self.type) + self.content + chr(3) + chr(3),
                      'utf-8'))
        except Exception as e:
            print("HERE: ", e)


def parseCMD():
    parser = argparse.ArgumentParser()
    parser.add_argument('hostname', help='Destination hostname or IP')
    parser.add_argument('port', help='Destination port', type=int)
    parser.add_argument('-f', dest='files', help='Input files to be sent',
                        nargs='+')
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

    # Check if we should use file mode or chat mode
    if (args.files):
        try:
            sock = socket.socket(socket.AF_INET, proto)
            # Connect to server and send data
            sock.connect((args.hostname, args.port))

            # Send files to server
            try:
                for file in args.files:
                    if os.stat(file).st_size > 0:
                        FHandle = open(file, "r")
                        data = package(0, FHandle.read())
                        # send contents
                        data.send(sock)
                        response = sock.recv(1024)
                        if response:
                            print("Response", response)
                        else:
                            print('no data received')
                    else:
                        print("Invalid file type, Skipping ({})".format(file))
            except OSError:
                print("Unable to open", file)

        except socket.error as e:
            print(e)

        finally:
            sock.close()
    else:
        try:
            sock = socket.socket(socket.AF_INET, proto)
            # Connect to server and send data
            sock.connect((args.hostname, args.port))

            # TODO Listen for typing and send it
            while True:
                usrInput = input("")
                data = package(1, usrInput)
                # send contents
                print("Made it 1")
                data.send(sock)
                print("Made it 2")
                response = sock.recv(1024)
                if response:
                    print("Response", response)
                else:
                    print('no data received')

        except Exception as e:
            print(e)

        finally:
            sock.close()


if __name__ == '__main__':
    try:
        main()
    except (KeyboardInterrupt, Exception):
        print("\nProgram exiting...")
