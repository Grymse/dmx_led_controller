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
import { watch } from 'vue';
import { onMounted } from 'vue';

const serialStore = useSerialStore();

const props = defineProps({
  currentSequence: {
    type: Object as () => protocol.Sequence,
    required: true,
  },
});

const emit = defineEmits(['save']);

const isConnected = computed(() => serialStore.isConnected);

const handlePlay = () => {
  const data = new protocol.Message({
    sequence: props.currentSequence,
  }).serializeBinary();
  serialStore.writeSerial(data);
};

const playAndSave = () => {
  const data = new protocol.Message({
    save_state: new protocol.State({
      sequence: props.currentSequence,
      settings: new protocol.Settings({
        group_id: 0,
        virtual_offset: 0,
      })
    }),
  }).serializeBinary();
  serialStore.writeSerial(data);
};

defineExpose({ handlePlay });
</script>
