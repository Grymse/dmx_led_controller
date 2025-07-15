<script setup lang="ts">
import { ref } from 'vue';
import Slider from 'primevue/slider';
import InputText from 'primevue/inputtext';
import Button from 'primevue/button';

const props = defineProps<{
  mask: { type: string; [key: string]: any };
  title: string;
}>();

const emit = defineEmits(['update:mask']);

// Handle updates to the mask
const updateMask = (updates: any) => {
  emit('update:mask', { ...props.mask, ...updates });
};

// Available mask types
const maskTypes = [
  { value: 'none', label: 'None' },
  { value: 'blink', label: 'Blink' },
  { value: 'invert', label: 'Invert' },
  { value: 'pulse', label: 'Pulse' },
  { value: 'sawtooth', label: 'Sawtooth' },
  { value: 'stars', label: 'Stars' },
  { value: 'wave', label: 'Wave' },
  { value: 'sectionsWave', label: 'Sections Wave' },
  { value: 'pulseSawtooth', label: 'Pulse Sawtooth' },
];

// Helper to ensure we have default values
const getNumericValue = (value: any, defaultValue: number): number => {
  if (value === undefined || value === null || isNaN(Number(value))) {
    return defaultValue;
  }
  return Number(value);
};
</script>

<template>
  <div>
    <div class="mb-4">
      <label class="block text-sm font-medium text-gray-700 mb-1">{{ title }} Type</label>
      <select
        class="w-full border border-gray-300 rounded-md p-2"
        v-model="mask.type"
        @change="updateMask({ type: $event.target.value })"
      >
        <option v-for="type in maskTypes" :key="type.value" :value="type.value">
          {{ type.label }}
        </option>
      </select>
    </div>

    <!-- Dynamic parameters based on mask type -->
    <div v-if="mask.type === 'blink'" class="mb-4">
      <!-- Blink parameters -->
      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Duration: {{ mask.duration || 500 }}ms</label>
        <div class="slider-container">
          <InputText v-model.number="mask.duration" type="number" min="50" max="5000" class="w-24" />
          <Slider v-model="mask.duration" class="slider-component" :min="50" :max="5000" :step="50" @change="updateMask({ duration: mask.duration })" />
        </div>
      </div>
    </div>

    <div v-else-if="mask.type === 'wave'" class="mb-4">
      <!-- Wave parameters -->
      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Duration: {{ mask.duration || 500 }}ms</label>
        <div class="slider-container">
          <InputText v-model.number="mask.duration" type="number" min="50" max="5000" class="w-24" />
          <Slider v-model="mask.duration" class="slider-component" :min="50" :max="5000" :step="50" @change="updateMask({ duration: mask.duration })" />
        </div>
      </div>

      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Length: {{ mask.length || 100 }}</label>
        <div class="slider-container">
          <InputText v-model.number="mask.length" type="number" min="1" max="500" class="w-24" />
          <Slider v-model="mask.length" class="slider-component" :min="1" :max="500" :step="5" @change="updateMask({ length: mask.length })" />
        </div>
      </div>

      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Gap: {{ mask.gap || 0 }}</label>
        <div class="slider-container">
          <InputText v-model.number="mask.gap" type="number" min="0" max="500" class="w-24" />
          <Slider v-model="mask.gap" class="slider-component" :min="0" :max="500" :step="5" @change="updateMask({ gap: mask.gap })" />
        </div>
      </div>
    </div>

    <div v-else-if="mask.type === 'pulse'" class="mb-4">
      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Duration: {{ mask.duration || 500 }}ms</label>
        <div class="slider-container">
          <InputText v-model.number="mask.duration" type="number" min="50" max="5000" class="w-24" />
          <Slider v-model="mask.duration" class="slider-component" :min="50" :max="5000" :step="50" @change="updateMask({ duration: mask.duration })" />
        </div>
      </div>
    </div>

    <div v-else-if="mask.type === 'stars'" class="mb-4">
      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Frequency: {{ mask.frequency || 100 }}</label>
        <div class="slider-container">
          <InputText v-model.number="mask.frequency" type="number" min="1" max="1000" class="w-24" />
          <Slider v-model="mask.frequency" class="slider-component" :min="1" :max="1000" :step="10" @change="updateMask({ frequency: mask.frequency })" />
        </div>
      </div>

      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Speed: {{ mask.speed || 30 }}</label>
        <div class="slider-container">
          <InputText v-model.number="mask.speed" type="number" min="1" max="100" class="w-24" />
          <Slider v-model="mask.speed" class="slider-component" :min="1" :max="100" :step="1" @change="updateMask({ speed: mask.speed })" />
        </div>
      </div>

      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Length: {{ mask.length || 1 }}</label>
        <div class="slider-container">
          <InputText v-model.number="mask.length" type="number" min="1" max="10" class="w-24" />
          <Slider v-model="mask.length" class="slider-component" :min="1" :max="10" :step="1" @change="updateMask({ length: mask.length })" />
        </div>
      </div>
    </div>

    <div v-else-if="mask.type === 'sawtooth'" class="mb-4">
      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Duration: {{ mask.duration || 500 }}ms</label>
        <div class="slider-container">
          <InputText v-model.number="mask.duration" type="number" min="50" max="5000" class="w-24" />
          <Slider v-model="mask.duration" class="slider-component" :min="50" :max="5000" :step="50" @change="updateMask({ duration: mask.duration })" />
        </div>
      </div>

      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Length: {{ mask.length || 300 }}</label>
        <div class="slider-container">
          <InputText v-model.number="mask.length" type="number" min="1" max="500" class="w-24" />
          <Slider v-model="mask.length" class="slider-component" :min="1" :max="500" :step="5" @change="updateMask({ length: mask.length })" />
        </div>
      </div>

      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Gap: {{ mask.gap || 150 }}</label>
        <div class="slider-container">
          <InputText v-model.number="mask.gap" type="number" min="0" max="500" class="w-24" />
          <Slider v-model="mask.gap" class="slider-component" :min="0" :max="500" :step="5" @change="updateMask({ gap: mask.gap })" />
        </div>
      </div>
    </div>

    <Button
      v-if="mask.type !== 'none'"
      label="Apply Changes"
      class="mt-4 w-full"
      @click="updateMask(mask)"
    />
  </div>
</template>

<style scoped>
/* Additional styling for slider components */
:deep(.p-slider) {
  margin-top: 0.5rem;
  margin-bottom: 0.5rem;
}

:deep(.p-inputtext) {
  padding: 0.5rem;
  font-size: 0.875rem;
}

.slider-container {
  display: flex;
  align-items: center;
  gap: 0.5rem;
  margin: 0.5rem 0;
  width: 100%;
}

.slider-component {
  flex: 1;
  max-width: calc(100% - 6rem);
  margin: 0 1.5rem 0 0.5rem; /* Add margin on both sides */
}
</style>
