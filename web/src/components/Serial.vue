<script setup lang="ts">
  import { ref, onMounted, onUnmounted } from 'vue';
  import { useSerialStore } from '../stores/serial.ts'; // Adjust path if nee
  import { protocol } from "../lib/protobuf/protocol.ts";
import { CRGB } from '@/protobuf-example.ts';

  const {
    Direction,
    Sequence,
    Animation,
    Layer,
    LayerType,
    Message
  } = protocol;

  const serialStore = useSerialStore();
  const dataToSend = ref('');

  // Optional: Automatically disconnect when the component is unmounted
  onUnmounted(() => {
    if (serialStore.isConnected) {
      serialStore.disconnectSerial();
    }
  });



  const animation2 = new Animation({
    direction: Direction.FORWARD,
    layers: [
      new Layer({
        type: LayerType.SwitchColor,
        colors: [
          CRGB(255, 0, 0),
          CRGB(0, 255, 0),
          CRGB(0, 0, 255),
        ],
        duration: 50,
      }),
    ],
    duration: 2000,
    first_tick: 0,
    brightness: 255,
  });

  const animation1 = new Animation({
    direction: Direction.FORWARD,
    layers: [
      new Layer({
        type: LayerType.SingleColor,
        color: CRGB(0, 255, 0),
      }),
    ],
    duration: 2000,
    first_tick: 0,
    brightness: 255,
  });

  function getSequence(first: boolean) {
    return Sequence.fromObject({
      animations: first ? [animation1] : [animation2],
    });
  }

  function sendProgram(program: number) {
    const data = new Message({
      sequence: getSequence(program !== 0),
    }).serializeBinary();
    serialStore.writeSerial(data);
  }

</script>

<template>
  <div class="app">
    <h1>Web Serial with Pinia</h1>

    <div class="controls">
      <button @click="serialStore.connectSerial" :disabled="serialStore.isConnected">
        Connect to Serial
      </button>
      <button @click="serialStore.disconnectSerial" :disabled="!serialStore.isConnected">
        Disconnect
      </button>
    </div>

    <button @click="sendProgram(0)">Send 1</button>
    <button @click="sendProgram(1)">Send 2</button>

    <div v-if="serialStore.error" class="error-message">
      {{ serialStore.error }}
    </div>

    <div class="received-data">
      <h2>Received Data:</h2>
      <pre>{{ serialStore.receivedData }}</pre>
    </div>
  </div>
</template>

<style scoped>
/* Add your CSS from the previous example here, or adapt for your Vue app */
.app {
  font-family: sans-serif;
  margin: 20px;
  background-color: #f4f4f4;
  color: #333;
}

h1 {
  color: #0056b3;
}

.controls, .send-data {
  margin-bottom: 20px;
  padding: 15px;
  border: 1px solid #ddd;
  border-radius: 8px;
  background-color: #fff;
}

button {
  padding: 10px 15px;
  margin-right: 10px;
  border: none;
  border-radius: 5px;
  background-color: #007bff;
  color: white;
  cursor: pointer;
  font-size: 16px;
}

button:disabled {
  background-color: #cccccc;
  cursor: not-allowed;
}

button:hover:not(:disabled) {
  background-color: #0056b3;
}

input[type="text"] {
  padding: 10px;
  width: 300px;
  border: 1px solid #ccc;
  border-radius: 5px;
  margin-right: 10px;
  font-size: 16px;
}

.received-data {
  background-color: #e9e9e9;
  padding: 15px;
  border-radius: 8px;
  border: 1px solid #ccc;
}

#receivedData {
  white-space: pre-wrap;
  word-wrap: break-word;
  max-height: 300px;
  overflow-y: auto;
  background-color: #fff;
  padding: 10px;
  border-radius: 5px;
  border: 1px solid #eee;
}

.error-message {
  color: red;
  background-color: #ffe0e0;
  border: 1px solid red;
  padding: 10px;
  margin-bottom: 15px;
  border-radius: 5px;
}
</style>
