# MQTT Client demo
# Continuously monitor two different MQTT topics for data,
# check if the received data matches two predefined 'commands'

import paho.mqtt.client as mqtt
import time
import open_url

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
    # convert from byte to string
    msg.payload = msg.payload.decode("UTF-8")

    # all the URLs
    # url_defualt = "https://img.freepik.com/free-photo/clean-cement-built-structure-white-background-copy-space_53876-30215.jpg?t=st=1648424008~exp=1648424608~hmac=b0cc0940184f22440953fe3a96965780a5b35ae07a6eda3717caae6abaac94c1&w=1480"
    url_defualt = "https://www.publicdomainpictures.net/pictures/30000/velka/plain-white-background.jpg"
    url_jar1 = "https://c.tenor.com/UQXB45tIezIAAAAC/put-that-down-stop.gif"
    url_jar2 = "https://c.tenor.com/c9dk20_6kAYAAAAC/put-it-down-jon-taffer.gif"

    if msg.payload == "Jar 1 removed!":
        print("Received message #1, do something")
        # open the defualt url -- plain white background
        open_url.browser.get(url_defualt)
        time.sleep(3)
        open_url.browser.get(url_jar1)
        time.sleep(15)
        open_url.browser.get(url_defualt)

    if msg.payload == "Jar 2 removed!":
        print("Received message #2, do something else")
        # open the defualt url -- plain white background
        open_url.browser.get(url_defualt)
        time.sleep(3)
        open_url.browser.get(url_jar2)
        time.sleep(15)
        open_url.browser.get(url_defualt)


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