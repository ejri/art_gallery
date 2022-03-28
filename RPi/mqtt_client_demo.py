# https://core-electronics.com.au/guides/getting-started-with-home-automation-using-mqtt/
# MQTT Client demo
# Continuously monitor two different MQTT topics for data,
# check if the received data matches two predefined 'commands'



import paho.mqtt.client as mqtt

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() - if we lose the connection and
    # reconnect then subscriptions will be renewed.

    # rename topics to match esp32 output
    client.subscribe("esp32/output")
    client.subscribe("esp32/Jar1")
    client.subscribe("esp32/Jar2")


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))
    #convert from byte to string
    msg.payload=msg.payload.decode('UTF-8')

    if msg.payload == "Jar 1 removed!":
        print("Received message #1, do something")
        # Do something

    if msg.payload == "Jar 2 removed!":
        print("Received message #2, do something else")
        # Do something else


# Create an MQTT client and attach our routines to it.
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("10.0.0.41", 1883, 60)

# Process network traffic and dispatch callbacks. This will also handle
# reconnecting. Check the documentation at
# https://github.com/eclipse/paho.mqtt.python
# for information on how to use other loop*() functions
client.loop_forever()