import argparse
import subprocess
from select import select
from typing import BinaryIO
import serial
import sys
import atexit

msglen = 32
cypherlen = 48

parser = argparse.ArgumentParser(description='Interactive terminal for my Cryptography project demo')
parser.add_argument('--encrypt', dest='enc', type=str, help='binary to encrypt data')
parser.add_argument('--decrypt', dest='dec', type=str, help='binary to decrypt data')

args = parser.parse_args()

encrypt = subprocess.Popen([args.enc], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
decrypt = subprocess.Popen([args.dec], stdin=subprocess.PIPE, stdout=subprocess.PIPE)

ser = serial.Serial('/dev/cu.ESSlave-DevB')  # open serial port

def onclose():
    ser.close()
    encrypt.kill()
    decrypt.kill()

atexit.register(onclose)

def read_n_encrypt(buffer: BinaryIO) -> bytes:
    command = buffer.readline(msglen)
    encrypt.stdin.write(command)
    encrypt.stdin.flush()
    encrypted = encrypt.stdout.read(cypherlen)
    print(f'Command received: {command}')
    hexencrypted = ''.join(format(x, '02x') for x in encrypted)
    print(f'Encrypted to (hex): {hexencrypted}')
    return encrypted

def decrypt_cypher(cypher: bytes) -> bytes: 
    print(len(cypher))
    wrote = decrypt.stdin.write(cypher)
    print(wrote)
    decrypt.stdin.flush()
    decrypted = decrypt.stdout.read(msglen)
    hexencrypted = ''.join(format(x, '02x') for x in cypher)
    print(f'Cypher is (hex): {hexencrypted}')
    print(f'Decrypted is: {str(decrypted)}')
    return decrypted

print('Ready!')
while True:
    if sys.stdin in select([sys.stdin], [], [], 0)[0]:
        encrypted = read_n_encrypt(sys.stdin.buffer)
        ser.write(encrypted)
    if ser.in_waiting:
        arduino_cypher = ser.read(48)
        decrypted = decrypt_cypher(arduino_cypher)
        

