<script lang="ts">
import {defineComponent} from 'vue'

export default defineComponent({
  name: "BluetoothMenu"
})
</script>

<template>
  <div class="flex flex-row items-center gap-4 px-4 py-1 bg-white/80 backdrop-blur-sm rounded-lg shadow-md">
    <!-- Connection Status Chip -->
    <div class="flex items-center gap-2">
      <span>Connection status:</span>
      <Chip :label="connectionStatus" :class="chipClass" />
    </div>

    <!-- Button Group for Controls -->
    <span class="p-buttonset">
      <Button class="mr-2" label="Play" icon="pi pi-play" @click="play" />
      <Button label="Play and save" icon="pi pi-save" @click="playAndSave" />
    </span>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue';
import Button from 'primevue/button';
import Chip from 'primevue/chip';

// Connection status
const isConnected = ref(true);
const connectionStatus = computed(() => isConnected.value ? 'Connected' : 'Disconnected');
const chipClass = computed(() => isConnected.value ? 'bg-green-500 text-white' : 'bg-red-500 text-white');

// Action methods
const play = () => {
  console.log('Play action triggered');
  // Implement your play logic here
};

const playAndSave = () => {
  console.log('Play and save action triggered');
  // Implement your play and save logic here
};
</script>

<style scoped>

</style>
