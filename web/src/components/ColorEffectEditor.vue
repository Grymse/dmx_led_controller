<script setup lang="ts">
import { ref, watch } from 'vue';
import Button from 'primevue/button';
import ColorPickerGroup from './ColorPickerGroup.vue';

const props = defineProps<{
  effect: { type: string; [key: string]: any };
}>();

const emit = defineEmits(['update:effect']);

// Initialize colors array if it doesn't exist
if (props.effect.type === 'fade' || props.effect.type === 'sections' || props.effect.type === 'sectionsWave' || props.effect.type === 'switch') {
  if (!props.effect.colors || !Array.isArray(props.effect.colors)) {
    props.effect.colors = ['#FF0000']; // Default red color
  }
} else if (props.effect.type === 'single' && !props.effect.color) {
  props.effect.color = '#FF0000'; // Default color for single color effect
}

// Handle updates to the effect
const updateEffect = (updates: any) => {
  emit('update:effect', { ...props.effect, ...updates });
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
      <label class="block text-sm font-medium text-gray-700 mb-1">Duration (ms)</label>
      <input
        type="number"
        class="w-full border border-gray-300 rounded-md p-2"
        min="1"
        v-model.number="effect.duration"
        @change="updateEffect({ duration: effect.duration })"
      />

      <label class="block text-sm font-medium text-gray-700 mt-2 mb-1">Length</label>
      <input
        type="number"
        class="w-full border border-gray-300 rounded-md p-2"
        min="1"
        v-model.number="effect.length"
        @change="updateEffect({ length: effect.length })"
      />
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
        <label class="block text-sm font-medium text-gray-700 mb-1">Duration (ms)</label>
        <input
          type="number"
          class="w-full border border-gray-300 rounded-md p-2"
          min="1"
          v-model.number="effect.duration"
          @change="updateEffect({ duration: effect.duration })"
        />
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
