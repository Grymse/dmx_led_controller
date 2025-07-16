// SerialConnectButton.vue
<template>
  <Button
    :label="buttonLabel"
    :icon="buttonIcon"
    :loading="isConnecting || isDisconnecting"
    @click="toggleConnection"
  />
</template>

<script setup lang="ts">
import { computed, ref } from 'vue';
import { useSerialStore } from '../stores/serial';

const serialStore = useSerialStore();

const isConnecting = ref(false);
const isDisconnecting = ref(false);

const buttonLabel = computed(() => {
  if (isConnecting.value) return 'Connecting...';
  if (isDisconnecting.value) return 'Disconnecting...';
  return serialStore.isConnected ? 'Disconnect' : 'Connect';
});

const buttonIcon = computed(() => {
  return serialStore.isConnected ? 'pi pi-power-off' : 'pi pi-link';
});

const buttonClass = computed(() => {
  return serialStore.isConnected ? 'p-button-danger' : 'p-button-success';
});

const toggleConnection = async () => {
  if (serialStore.isConnected) {
    isDisconnecting.value = true;
    try {
      await serialStore.disconnectSerial();
    } finally {
      isDisconnecting.value = false;
    }
  } else {
    isConnecting.value = true;
    try {
      await serialStore.connectSerial();
    } finally {
      isConnecting.value = false;
    }
  }
};
</script>
