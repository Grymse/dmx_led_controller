<script setup lang="ts">
import { ref, watch, computed } from 'vue';
import Button from 'primevue/button';
import Slider from 'primevue/slider';
import InputText from 'primevue/inputtext';
import ColorPickerGroup from './ColorPickerGroup.vue';

const props = defineProps<{
  effect: { type: string; [key: string]: any };
}>();

const emit = defineEmits(['update:effect']);

// Define the configuration for each effect type
const effectConfigs = {
  'single': {
    label: 'Single Color',
    id: 0, // SingleColor = 0
    params: [
      { key: 'color', type: 'color', default: '#FF0000', required: true },
    ]
  },
  'rainbow': {
    label: 'Rainbow',
    id: 1, // RainbowColor = 1
    params: [
      { key: 'duration', type: 'slider', default: 50, min: 1, max: 1000, step: 5, label: 'Duration', unit: 'ms', required: true },
      { key: 'length', type: 'slider', default: 150, min: 1, max: 500, step: 5, label: 'Length', required: true }
    ]
  },
  'sectionsWave': {
    label: 'Sections Wave',
    id: 2, // SectionsWaveColor = 2
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 300, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'sections': {
    label: 'Sections',
    id: 3, // SectionsColor = 3
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 300, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'fade': {
    label: 'Fade',
    id: 4, // FadeColor = 4
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 300, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  },
  'switch': {
    label: 'Switch',
    id: 5, // SwitchColor = 5
    params: [
      { key: 'colors', type: 'colors', default: ['#FF0000'], required: true },
      { key: 'duration', type: 'slider', default: 300, min: 50, max: 5000, step: 50, label: 'Duration', unit: 'ms', required: true }
    ]
  }
};

// Generate effectTypes array for select dropdown
const effectTypes = Object.entries(effectConfigs).map(([value, config]) => ({
  value,
  label: config.label
}));

// Get current effect configuration
const currentEffectConfig = computed(() => {
  return effectConfigs[props.effect.type] || null;
});

// Ensure effect has required parameters when type changes
const initializeEffect = () => {
  const config = currentEffectConfig.value;
  if (!config) return;

  const updates: Record<string, any> = {};
  let hasUpdates = false;

  config.params.forEach(param => {
    // Check if parameter exists and is valid
    if (param.key === 'color' && (props.effect.color === undefined || typeof props.effect.color !== 'string')) {
      updates.color = param.default;
      hasUpdates = true;
    } else if (param.key === 'colors' && (!props.effect.colors || !Array.isArray(props.effect.colors) || props.effect.colors.length === 0)) {
      updates.colors = [...param.default];
      hasUpdates = true;
    } else if (param.key !== 'color' && param.key !== 'colors' && props.effect[param.key] === undefined) {
      updates[param.key] = param.default;
      hasUpdates = true;
    }
  });

  if (hasUpdates) {
    updateEffect(updates);
  }
};

// Initialize on component creation
initializeEffect();

// Handle updates to the effect
const updateEffect = (updates: any) => {
  const updatedEffect = { ...props.effect, ...updates };
  emit('update:effect', updatedEffect);
};

// Handle color updates from ColorPickerGroup
const updateColors = (colors: string[] | string) => {
  if (props.effect.type === 'single') {
    updateEffect({ color: colors });
  } else if (Array.isArray(colors)) {
    updateEffect({ colors });
  }
};

// Watch for effect type changes to initialize properties
watch(() => props.effect.type, (newType) => {
  initializeEffect();
});
</script>

<template>
  <div>
    <div class="mb-4">
      <label class="block text-sm font-medium text-gray-700 mb-1">Effect Type</label>
      <select
        class="w-full border border-gray-300 rounded-md p-2"
        v-model="effect.type"
        @change="updateEffect({ type: $event.target.value })"
      >
        <option v-for="type in effectTypes" :key="type.value" :value="type.value">
          {{ type.label }}
        </option>
      </select>
    </div>

    <!-- Dynamic parameters based on effect type -->
    <div v-if="currentEffectConfig" class="mb-4">
      <template v-for="param in currentEffectConfig.params" :key="param.key">
        <!-- Color Parameter -->
        <div v-if="param.key === 'color'" class="mb-3">
          <label class="block text-sm font-medium text-gray-700 mb-1">Color</label>
          <ColorPickerGroup
            :colors="effect.color || param.default"
            :multiple="false"
            @update:colors="updateColors"
          />
        </div>

        <!-- Multiple Colors Parameter -->
        <div v-else-if="param.key === 'colors'" class="mb-3">
          <label class="block text-sm font-medium text-gray-700 mb-1">Colors</label>
          <ColorPickerGroup
            :colors="effect.colors || param.default"
            :multiple="true"
            @update:colors="updateColors"
          />
        </div>

        <!-- Slider Parameter -->
        <div v-else-if="param.type === 'slider'" class="mb-3">
          <label class="block text-sm font-medium text-gray-700 mb-1">
            {{ param.label }}: {{ effect[param.key] || param.default }}{{ param.unit || '' }}
          </label>
          <div class="slider-container">
            <InputText
              v-model.number="effect[param.key]"
              type="number"
              :min="param.min"
              :max="param.max"
              class="w-24"
            />
            <Slider
              v-model="effect[param.key]"
              class="slider-component"
              :min="param.min"
              :max="param.max"
              :step="param.step"
              @change="updateEffect({ [param.key]: effect[param.key] })"
            />
          </div>
        </div>
      </template>
    </div>

    <!-- Apply button -->
    <Button
      label="Apply Changes"
      class="mt-4 w-full"
      @click="emit('update:effect', effect)"
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
