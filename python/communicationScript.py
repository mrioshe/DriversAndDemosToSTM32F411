import subprocess

# Assuming other_script.py is in the same directory
script_path = "./communicationFunctions.py"

# Call the script using subprocess
subprocess.call(["python", script_path])

ser = serial.Serial('/dev/ttyACM0', 921600)
data1 = 0
data2 = 0

while True:
    data1 = ser.read(1)
    if data1.decode('utf-8') == 's':
        print("Comenzando comunicación")
        for i in range(len(cleaned_matrix.iloc[0])-1):
            dataAux=str(cleaned_matrix.iloc[0][i])
            ser.write(dataAux.encode())
            while True:
                data2=ser.read(1)
                if (data2.decode('utf-8')=='c'):
                    print('Dato ', i, ' de la fila ', 0, ' leído por el micro. El dato es: ', dataAux.encode())
                    break;
        print("La comunicación finalizó")   
        ser.close()
        break