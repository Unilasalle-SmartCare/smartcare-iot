import base64
import json

import paho.mqtt.client as mqtt
import pylint
import pyodbc
import datetime

#Call back functions 

# gives connection message
def on_connect(mqttc, mosq, obj,rc):
    print("Connected with result code:"+str(rc))
    # subscribe for all devices of user
    mqttc.subscribe('home/#')

# gives message from device
def on_message(mqttc,obj,msg):
    print("Mensagem recebida: ")
    print(msg.payload.decode('utf-8'))
    print("Topico: "+ msg.topic)
    #rssi = msg["metadata"]["gateways"]["rssi"]  # <== this raises an error (why?)
    rssi = -1
    Topico = str(msg.topic)
    Payload = str(msg.payload.decode('utf-8'))
    print("--------Topico---------")
    print(Topico)
    print("-------Payload---------")
    print(Payload)
    print("-----------------------")
    x = json.loads(msg.payload)
    Topic_Handle(str(msg.topic), Payload)

def on_log(mqttc,obj,level,buf):
    print("message:" + str(buf))
    #print("mqttc:" + str(mqttc))
    #print("level:" + str(level))
    print("userdata:" + str(obj))

def on_publish(mosq, obj, mid):
    print("mid: " + str(mid))

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))
    

#===============================================================
# Function to Handle Data into Database


def Topic_Handle(topic, value):
    topic_array = topic.split("/")
    if topic_array[2] == "sensor":
        print(topic_array)
        CodigoDispositivo =  topic_array[3]
        print(CodigoDispositivo)
        DataHora = datetime.datetime.today()
        Valor = value
        try:
            #;UID=luiza;PWD=password
            conn = pyodbc.connect('DRIVER={ODBC Driver 17 for SQL Server};SERVER=(localdb)\MSSQLLocalDB;DATABASE=smartcare_db')
            cur = conn.cursor()
            cur.execute("insert into Medicao (CodigoDispositivo, DataHora, Valor) values (?,?,?)",[CodigoDispositivo, DataHora, Valor])
            conn.commit()
            conn.close()
            print ("Medição cadastrada no Banco de Dados.")        
            print ("")
        except Exception as ex:
            print(ex.__cause__)
            print(ex)
            #Exception(ex.__cause__, "Ocorreu um erro.")    

#===============================================================

mqttc= mqtt.Client("sql_handle")
mqttc.on_connect=on_connect
mqttc.on_message=on_message
mqttc.on_log=on_log
mqttc.on_publish=on_publish
mqttc.on_subscribe=on_subscribe

mqttc.username_pw_set("smartcare","unilasalle")

mqttc.connect("ec2-18-209-178-192.compute-1.amazonaws.com")

# and listen to server
run = True
while run:
    mqttc.loop()

