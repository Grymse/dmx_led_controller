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
  program: {
    type: Number,
    default: 0,
  },
});

const emit = defineEmits(['save']);

const currentSequence = computed<protocol.Sequence>(() => {
  // Replace this with your actual sequence generation logic
  return { animations: [] };
});

const isConnected = computed(() => serialStore.isConnected);

function getSequence(program: boolean): protocol.Sequence {
  // Replace this with your actual sequence generation logic
  return { animations: [] };
}

function sendProgram(program: number) {
  const data = new protocol.Message({
    sequence: getSequence(program !== 0),
  }).serializeBinary();
  serialStore.writeSerial(data);
}

const handlePlay = () => {
  sendProgram(props.program);
};

const playAndSave = () => {
  emit('save');
};
</script>
