import paho.mqtt.client as mqtt
import json
import base64
import pyodbc
import pylint

#Call back functions 

# gives connection message
def on_connect(mqttc, mosq, obj,rc):
    print("Connected with result code:"+str(rc))
    # subscribe for all devices of user
    mqttc.subscribe('home/#')

# gives message from device
def on_message(mqttc,obj,msg):
    print("Mensagem recebida: "+str(msg.payload))
    print("Topico: "+str(msg.topic))
    rssi = msg["metadata"]["gateways"]["rssi"]  # <== this raises an error (why?)
    rssi = -1
    Topico = str(msg.topic)
    Payload = str(msg.payload)
    x = json.loads(msg.payload)
    Topic_Handle(Topico, Payload)      

def on_log(mqttc,obj,level,buf):
    print("message:" + str(buf))
    print("userdata:" + str(obj))

def on_publish(mosq, obj, mid):
    print("mid: " + str(mid))

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))
    



#===============================================================
# Database Manager Class
CONNECTION_STRING = "YourConnectionString"

class DatabaseManager():
	def __init__(self):
		self.conn = pyodbc.connect(CONNECTION_STRING)
		self.cur = self.conn.cursor()
		
	def add_del_update_db_record(self, sql_query, args=()):
		self.cur.execute(sql_query, args)
		self.conn.commit()
		return

	def __del__(self):
		self.cur.close()
		self.conn.close()

#===============================================================
# Function to Handle Data into Database


def Topic_Handle(topic, msg):
    if topic == "home/quarto/+/+/dados":
        device = {}
        device.IdDispositivo = msg['IdDispositivo']
        device.IdTipo = msg['IdTipo']
        device.IdAmbiente = msg['device.IdAmbiente']
        device.Nome = msg['Nome']
        device.Descricao = msg['Descricao']
        device.Eixo_X = msg['Eixo_X']
        device.Eixo_Y = msg['Eixo_Y'] 

        Data_Handler(device, "registro")

    elif topic == "home/quarto/+/+/pwr":
        medicao = {}
        medicao.IdDispositivo = msg['IdDispositivo']
        medicao.DataHora = msg['DataHora']
        medicao.Valor = msg['Valor']
        medicao.Unidade = msg['Unidade']

        Data_Handler(device, "medicao")  

def Data_Handler(obj, tipo):		
	#Push into DB Table
    try:
        if tipo == "registro":
            
            #Parse Data 
            IdDispositivo = obj.IdDispositivo
            IdTipo = obj.IdTipo
            IdAmbiente = obj.IdAmbiente
            Nome = obj.Nome
            Descricao = obj.Descricao
            Eixo_X = obj.Eixo_X
            Eixo_Y = obj.Eixo_Y

            dbObj = DatabaseManager()
            dbObj.add_del_update_db_record("insert into Dispositivo (IdDispositivo, IdTipo, IdAmbiente, Nome, Descrição, Eixo_X. Eixo_Y) values (?,?,?,?,?,?,?)",[IdDispositivo, IdTipo, IdAmbiente, Nome, Descricao, Eixo_X, Eixo_Y])
            
            print ("Dados do Dispositivo inseridos no Banco de Dados.")        
            print ("")

        elif tipo == "medicao":
            IdDispositivo = obj.IdDispositivo
            DataHora = obj.DataHora
            Valor = obj.Valor
            Unidade = obj.Unidade

            dbObj = DatabaseManager()
            dbObj.add_del_update_db_record("insert into Medicao (IdDispositivo, DataHora, Valor, Unidade) values (?,?,?,?)",[IdDispositivo, DataHora, Valor, Unidade])
            
            print ("Medição cadastrada no Banco de Dados.")        
            print ("")
            
    except Exception as ex:
        Exception(ex.__cause__, "Ocorreu um erro so registrar Dispositivo.") 
    finally:
        del dbObj

#===============================================================


mqttc= mqtt.Client()
mqttc.on_connect=on_connect
mqttc.on_message=on_message
mqttc.on_log=on_log
mqttc.on_publish=on_publish
mqttc.on_subscribe=on_subscribe

mqttc.username_pw_set("luiza","quarto")

# BEWARE, outdated; see tstaging.thethingsnetwork.orghe follow up posts to use eu.thethings.network instead
mqttc.connect("my_broker",1883,60)

# and listen to server
run = True
while run:
    mqttc.loop()

