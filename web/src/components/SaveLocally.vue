// SaveLocally.vue
<template>
  <div class="flex gap-2">
    <Button
      label="Save Locally"
      icon="pi pi-download"
      severity="secondary"
      outlined
      @click="saveModules"
    />
    <Button
      label="Load Program"
      icon="pi pi-upload"
      severity="secondary"
      outlined
      @click="loadSequence"
    />
    <input type="file" id="fileInput" @change="handleFileChange" style="display: none" accept=".json">
  </div>
</template>

<script setup lang="ts">
import { defineProps, defineEmits } from 'vue';
import Button from 'primevue/button';
import type { Module } from './lib/module';

const props = defineProps({
  modules: {
    type: Array as () => Module[],
    required: true,
  },
});

const emit = defineEmits(['updateModules']);

const saveModules = () => {
  const jsonData = JSON.stringify(props.modules, null, 2);
  const blob = new Blob([jsonData], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'modules.json';
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
  URL.revokeObjectURL(url);
};

const loadSequence = () => {
  const fileInput = document.getElementById('fileInput') as HTMLInputElement;
  fileInput.click();
};

const handleFileChange = (event: Event) => {
  const fileInput = event.target as HTMLInputElement;
  if (fileInput.files && fileInput.files.length > 0) {
    const file = fileInput.files[0];
    const reader = new FileReader();

    reader.onload = (e: ProgressEvent<FileReader>) => {
      try {
        const jsonData = JSON.parse(reader.result as string);
        emit('updateModules', jsonData);
      } catch (error) {
        console.error('Error parsing JSON:', error);
      }
    };

    reader.readAsText(file);
  }
};
</script>
