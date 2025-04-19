import aiomqtt
import json

class Node:
    def __init__(self, mac_addr: str, client: aiomqtt.Client):
        self.mac_addr = mac_addr
        self.client = client

    async def send_data(self, data):
        await self.client.publish(f'/topic/{self.mac_addr}', json.dumps(data))
