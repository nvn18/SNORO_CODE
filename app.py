import serial
import pandas as pd

ser=serial.Serial("COM5",9600,timeout=0.5)
dataset=[]
batch=0

while True:
    try:
        if (ser.inWaiting()>0):
            data=ser.readline()
            data=data.decode('utf-8')
            if data.startswith("#"):
                data=data.split(",")
                vib=int(data[1])
                h=float(data[2])
                t=float(data[3])
                heartRate=int(data[4])
                result=data[5]
                dummy=[vib,h,t,heartRate,result]
                batch+=1
                dataset.append(dummy)
                print(dataset)
                if batch==10:
                    df=pd.DataFrame(dataset)
                    df.to_csv("dataset.csv")
                    batch=0
    except:
        continue