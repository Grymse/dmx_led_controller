<script setup lang="ts">
import { ref, computed } from 'vue';
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

// Define the configuration for each mask type
const maskConfigs = {
  'none': {
    label: 'None',
    id: null,
    params: []
  },
  'blink': {
    label: 'Blink',
    id: 50, // BlinkMask = 50
    params: [
      { key: 'duration', type: 'slider', default: 500, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'invert': {
    label: 'Invert',
    id: 51, // InvertMask = 51
    params: [] // No parameters needed for invert
  },
  'pulseSawtooth': {
    label: 'Pulse Sawtooth',
    id: 52, // PulseSawtoothMask = 52
    params: [
      { key: 'gap', type: 'slider', default: 150, min: 0, max: 500, step: 5, label: 'Gap', required: true },
      { key: 'duration', type: 'slider', default: 500, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'pulse': {
    label: 'Pulse',
    id: 53, // PulseMask = 53
    params: [
      { key: 'duration', type: 'slider', default: 500, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'sawtooth': {
    label: 'Sawtooth',
    id: 54, // SawtoothMask = 54
    params: [
      { key: 'length', type: 'slider', default: 300, min: 1, max: 500, step: 5, label: 'Length', required: true },
      { key: 'gap', type: 'slider', default: 150, min: 0, max: 500, step: 5, label: 'Gap', required: true },
      { key: 'duration', type: 'slider', default: 500, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'sectionsWave': {
    label: 'Sections Wave',
    id: 55, // SectionsWaveMask = 55
    params: [
      { key: 'duration', type: 'slider', default: 500, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'sections': {
    label: 'Sections',
    id: 56, // SectionsMask = 56
    params: [
      { key: 'duration', type: 'slider', default: 500, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'stars': {
    label: 'Stars',
    id: 57, // StarsMask = 57
    params: [
      { key: 'frequency', type: 'slider', default: 100, min: 1, max: 1000, step: 10, label: 'Frequency', required: true },
      { key: 'speed', type: 'slider', default: 30, min: 1, max: 100, step: 1, label: 'Speed', required: true },
      { key: 'length', type: 'slider', default: 1, min: 1, max: 10, step: 1, label: 'Length', required: true }
    ]
  },
  'wave': {
    label: 'Wave',
    id: 58, // WaveMask = 58
    params: [
      { key: 'length', type: 'slider', default: 100, min: 1, max: 500, step: 5, label: 'Length', required: true },
      { key: 'gap', type: 'slider', default: 0, min: 0, max: 500, step: 5, label: 'Gap', required: true },
      { key: 'duration', type: 'slider', default: 500, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  }
};

// Generate maskTypes array for select dropdown
const maskTypes = Object.entries(maskConfigs).map(([value, config]) => ({
  value,
  label: config.label
}));

// Get current mask configuration
const currentMaskConfig = computed(() => {
  return maskConfigs[props.mask.type] || maskConfigs.none;
});

// Ensure mask has required parameters when type changes
const initializeMask = () => {
  const config = currentMaskConfig.value;

  const updates: Record<string, any> = {};
  let hasUpdates = false;

  config.params.forEach(param => {
    // Check if parameter exists
    if (props.mask[param.key] === undefined) {
      updates[param.key] = param.default;
      hasUpdates = true;
    }
  });

  if (hasUpdates) {
    updateMask(updates);
  }
};

// Helper to ensure we have default values
const getNumericValue = (value: any, defaultValue: number): number => {
  if (value === undefined || value === null || isNaN(Number(value))) {
    return defaultValue;
  }
  return Number(value);
};

// Initialize mask parameters on component creation
initializeMask();
</script>

<template>
  <div>
    <div class="mb-4">
      <label class="block text-sm font-medium text-gray-700 mb-1">{{ title }} Type</label>
      <select
        class="w-full border border-gray-300 rounded-md p-2"
        v-model="mask.type"
        @change="updateMask({ type: $event.target.value }); initializeMask();"
      >
        <option v-for="type in maskTypes" :key="type.value" :value="type.value">
          {{ type.label }}
        </option>
      </select>
    </div>

    <!-- Dynamic parameters based on mask type -->
    <div v-if="mask.type !== 'none'" class="mb-4">
      <!-- Render each parameter based on its type -->
      <div v-for="param in currentMaskConfig.params" :key="param.key" class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">
          {{ param.label }}: {{ mask[param.key] || param.default }}{{ param.unit || '' }}
        </label>
        <div class="slider-container">
          <InputText
            v-model.number="mask[param.key]"
            type="number"
            :min="param.min"
            :max="param.max"
            class="w-24"
          />
          <Slider
            v-model="mask[param.key]"
            class="slider-component"
            :min="param.min"
            :max="param.max"
            :step="param.step"
            @change="updateMask({ [param.key]: mask[param.key] })"
          />
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
