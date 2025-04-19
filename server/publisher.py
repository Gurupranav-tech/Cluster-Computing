import asyncio
import aiomqtt
import sys
from nodes import Node

BROKER = "192.168.151.195"
PORT = 1883
IDENTITY_TOPIC = "/topic/identity"

nodes: list[Node] = []

async def create_connection():
    async with aiomqtt.Client(BROKER, PORT) as client:
        print("Connected to EMQX Broker")
        await client.subscribe(IDENTITY_TOPIC)

        async for message in client.messages:
            if message.topic.matches(IDENTITY_TOPIC):
                print(f'Connection from MAC {message.payload}')
                nodes.append(Node(message.payload.decode(), client))


async def publish_task():
    try:
        while True:
            data = await asyncio.to_thread(input, ">")
            coro = []
            for node in nodes:
                coro.append(node.send_data({ "data": data }))
            await asyncio.gather(*coro)
    except KeyboardInterrupt as exec:
        return

async def main():
    conn_task = asyncio.create_task(create_connection())
    pub_task = asyncio.create_task(publish_task())

    await asyncio.gather(pub_task, conn_task)

if __name__ == "__main__":
    asyncio.run(main())
