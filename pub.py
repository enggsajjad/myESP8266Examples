import paho.mqtt.client as mqtt

broker_url = "iot.eclipse.org"
broker_port = 1883

client = mqtt.Client()
client.connect(broker_url, broker_port)

client.publish(topic="/esp/test", payload="TestingPayload from Sajjad", qos=1, retain=False)