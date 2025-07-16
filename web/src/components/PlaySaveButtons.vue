// PlaySaveButtons.vue
<template>
  <div class="flex">
    <Button
      class="mr-2"
      label="Play"
      icon="pi pi-play"
      :disabled="!isConnected"
      @click="handlePlay"
    />
    <Button
      label="Save to lamp"
      icon="pi pi-save"
      :disabled="!isConnected"
      @click="playAndSave"
    />
  </div>
</template>

<script setup lang="ts">
import { useSerialStore } from '../stores/serial';
import { computed } from 'vue';
import { protocol } from '../lib/protobuf/protocol';

const serialStore = useSerialStore();

const props = defineProps({
  currentSequence: {
    type: Object as () => protocol.Sequence,
    required: true,
  },
});

const emit = defineEmits(['save']);


const isConnected = computed(() => serialStore.isConnected);

function sendProgram() {
  const data = new protocol.Message({
    sequence: props.currentSequence,
  }).serializeBinary();
  serialStore.writeSerial(data);
}

const handlePlay = () => {
  sendProgram();
};

const playAndSave = () => {
  emit('save');
};
</script>
