import pyaudio
import socket
import wave
import sys

#troque o ip abaixo e defina a porta desejada, acima de 1024
#my room
UDP_IP = "192.168.0.164"

#following for teco
#UDP_IP = "192.168.10.121"
UDP_PORT = 1234

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

p = pyaudio.PyAudio()
stream = p.open(format=32, channels=1, rate=8000, output=True)
frames = []
cntr = 1

try:
    while True:
        data, addr = sock.recvfrom(1024)
        cntr = cntr + 1
        stream.write(data)
        frames.append(data)
        #if (cntr > 40):
        #    break
        
except KeyboardInterrupt:
    print("Finalizado com Ctrl+C")
    stream.stop_stream()
    stream.close()
    p.terminate()
    waveFile = wave.open("test.wav", 'wb')
    waveFile.setnchannels(1)
    waveFile.setsampwidth(p.get_sample_size(32))
    waveFile.setframerate(8000)
    waveFile.writeframes(b''.join(frames))
    waveFile.close()