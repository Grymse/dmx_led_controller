// SaveLocally.vue
<template>
  <div class="flex gap-2">
    <Button
      label="Save Locally"
      icon="pi pi-download"
      severity="secondary"
      outlined
      @click="openSaveDialog"
    />
    <Button
      label="Load Program"
      icon="pi pi-upload"
      severity="secondary"
      outlined
      @click="loadSequence"
    />
    <input type="file" id="fileInput" @change="handleFileChange" style="display: none" accept=".json">

    <Dialog v-model:visible="saveDialogVisible" modal header="Save Sequence" :style="{ width: '300px' }">
      <div class="flex flex-col gap-1 overflow-auto">
        <label for="filename">Filename:</label>
        <InputText id="filename" v-model="filename" type="text" />
      </div>
      <template #footer>
        <Button label="Cancel" icon="pi pi-times" @click="saveDialogVisible = false" class="p-button-text" />
        <Button label="Save" icon="pi pi-check" @click="saveSequence" autofocus />
      </template>
    </Dialog>
  </div>
</template>

<script setup lang="ts">
import { ref, defineProps, defineEmits } from 'vue';
import Button from 'primevue/button';
import Dialog from 'primevue/dialog';
import InputText from 'primevue/inputtext';
import type { Module } from '@/lib/module';

const props = defineProps({
  currentSequence: {
    type: Array as () => Module[],
    required: true,
  },
});

const emit = defineEmits(['updateSequence']);

const filename = ref('sequence');
const saveDialogVisible = ref(false);

const openSaveDialog = () => {
  saveDialogVisible.value = true;
};

const saveSequence = () => {
  // Convert the modules array to a JSON string
  const jsonData = JSON.stringify(props.currentSequence, null, 2);
  const blob = new Blob([jsonData], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = filename.value + '.json';
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
  URL.revokeObjectURL(url);
  saveDialogVisible.value = false;
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
