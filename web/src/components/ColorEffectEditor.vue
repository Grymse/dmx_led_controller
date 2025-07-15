<script setup lang="ts">
import { ref, watch } from 'vue';
import Button from 'primevue/button';
import Slider from 'primevue/slider';
import InputText from 'primevue/inputtext';
import ColorPickerGroup from './ColorPickerGroup.vue';

const props = defineProps<{
  effect: { type: string; [key: string]: any };
}>();

const emit = defineEmits(['update:effect']);

// Ensure colors are properly initialized
const initializeEffect = () => {
  if (props.effect.type === 'fade' || props.effect.type === 'sections' || props.effect.type === 'sectionsWave' || props.effect.type === 'switch') {
    if (!props.effect.colors || !Array.isArray(props.effect.colors)) {
      props.effect.colors = ['#FF0000']; // Default red color
    } else {
      // Ensure all colors have # prefix
      props.effect.colors = props.effect.colors.map((color: string) => {
        return color.startsWith('#') ? color : `#${color}`;
      });
    }
  } else if (props.effect.type === 'single' && !props.effect.color) {
    props.effect.color = '#FF0000'; // Default color for single color effect
  } else if (props.effect.type === 'single' && typeof props.effect.color === 'string') {
    // Ensure single color has # prefix
    props.effect.color = props.effect.color.startsWith('#')
      ? props.effect.color
      : `#${props.effect.color}`;
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

// Available effect types
const effectTypes = [
  { value: 'single', label: 'Single Color' },
  { value: 'rainbow', label: 'Rainbow' },
  { value: 'fade', label: 'Fade' },
  { value: 'sections', label: 'Sections' },
  { value: 'sectionsWave', label: 'Sections Wave' },
  { value: 'switch', label: 'Switch' },
];

// Watch for effect type changes to initialize properties
watch(() => props.effect.type, (newType) => {
  if (newType === 'single' && !props.effect.color) {
    updateEffect({ color: '#FF0000' });
  } else if ((newType === 'fade' || newType === 'sections' || newType === 'sectionsWave' || newType === 'switch')
            && (!props.effect.colors || !Array.isArray(props.effect.colors))) {
    updateEffect({ colors: ['#FF0000'] });
  }
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

    <!-- Single color effect -->
    <div v-if="effect.type === 'single'" class="mb-4">
      <label class="block text-sm font-medium text-gray-700 mb-1">Color</label>
      <ColorPickerGroup
        :colors="effect.color"
        :multiple="false"
        @update:colors="updateColors"
      />
    </div>

    <!-- Rainbow parameters -->
    <div v-else-if="effect.type === 'rainbow'" class="mb-4">
      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Duration: {{ effect.duration || 50 }}ms</label>
        <div class="slider-container">
          <InputText v-model.number="effect.duration" type="number" min="1" max="1000" class="w-24" />
          <Slider v-model="effect.duration" class="slider-component" :min="1" :max="1000" :step="5" @change="updateEffect({ duration: effect.duration })" />
        </div>
      </div>

      <div class="mb-3">
        <label class="block text-sm font-medium text-gray-700 mb-1">Length: {{ effect.length || 150 }}</label>
        <div class="slider-container">
          <InputText v-model.number="effect.length" type="number" min="1" max="500" class="w-24" />
          <Slider v-model="effect.length" class="slider-component" :min="1" :max="500" :step="5" @change="updateEffect({ length: effect.length })" />
        </div>
      </div>
    </div>

    <!-- Multiple colors effects (fade, sections, sectionsWave, switch) -->
    <div v-else-if="['fade', 'sections', 'sectionsWave', 'switch'].includes(effect.type)" class="mb-4">
      <label class="block text-sm font-medium text-gray-700 mb-1">Colors</label>

      <ColorPickerGroup
        :colors="effect.colors"
        :multiple="true"
        @update:colors="updateColors"
      />

      <!-- Duration for fade/switch effects -->
      <div v-if="['fade', 'switch'].includes(effect.type)" class="mt-4">
        <label class="block text-sm font-medium text-gray-700 mb-1">Duration: {{ effect.duration || 300 }}ms</label>
        <div class="slider-container">
          <InputText v-model.number="effect.duration" type="number" min="50" max="5000" class="w-24" />
          <Slider v-model="effect.duration" class="slider-component" :min="50" :max="5000" :step="50" @change="updateEffect({ duration: effect.duration })" />
        </div>
      </div>
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
