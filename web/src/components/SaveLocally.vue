// SaveLocally.vue
<template>
  <div class="flex gap-2">
    <Button
      label="Save Locally"
      icon="pi pi-download"
      severity="secondary"
      outlined
      @click="saveSequence"
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
import type { Module } from '@/lib/module';

const props = defineProps({
  currentSequence: {
    type: Array as () => Module[],
    required: true,
  },
});

const emit = defineEmits(['updateSequence']);

const saveSequence = () => {
  // Convert the modules array to a JSON string
  const jsonData = JSON.stringify(props.currentSequence, null, 2);
  const blob = new Blob([jsonData], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'sequence.json';
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
        const jsonData = e.target?.result as string;
        const sequenceData = JSON.parse(jsonData);
        emit('updateSequence', sequenceData);
      } catch (error) {
        console.error('Error loading sequence:', error);
        alert('Failed to load sequence.  Please check the file format.');
      }
    };

    reader.readAsText(file);
  }
};
</script>
