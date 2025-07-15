<script setup lang="ts">
import { ref, defineProps, defineEmits, watch, computed } from 'vue';

const props = defineProps<{
  id: number;
  name: string;
  isActive: boolean;
  duration: number;
  colorEffect?: { type: string; [key: string]: any };
  index: number; // Add index prop to know position
}>();

const emit = defineEmits([
  'click',
  'remove',
  'update:duration',
  'dragstart',
  'dragover',
  'drop'
]);

const localDuration = ref(props.duration);

// Update local duration when prop changes
watch(() => props.duration, (newDuration) => {
  localDuration.value = newDuration;
});

const handleClick = () => {
  emit('click', props.id);
};

const handleRemove = (e: Event) => {
  e.stopPropagation();
  emit('remove', props.id);
};

const updateDuration = (e: Event) => {
  e.stopPropagation(); // Prevent module selection when interacting with duration
  const value = parseInt((e.target as HTMLSelectElement).value);
  emit('update:duration', props.id, value);
};

// Drag and drop handlers
const handleDragStart = (e: DragEvent) => {
  if (e.dataTransfer) {
    e.dataTransfer.effectAllowed = 'move';
    // Store the dragged module's index
    e.dataTransfer.setData('text/plain', props.index.toString());
    // Add a class to style the dragging element
    if (e.target instanceof HTMLElement) {
      e.target.classList.add('dragging');
    }
  }
  emit('dragstart', props.index);
};

const handleDragEnd = (e: DragEvent) => {
  if (e.target instanceof HTMLElement) {
    e.target.classList.remove('dragging');
  }
};

const handleDragOver = (e: DragEvent) => {
  e.preventDefault(); // Necessary to allow dropping
  emit('dragover', props.index);
};

const handleDrop = (e: DragEvent) => {
  e.preventDefault();
  if (e.dataTransfer) {
    const fromIndex = parseInt(e.dataTransfer.getData('text/plain'));
    emit('drop', { fromIndex, toIndex: props.index });
  }
};

// Helper function to ensure color is in valid format
const formatColorIfNeeded = (color: string) => {
  if (typeof color !== 'string') return '#FF0000';

  if (color.startsWith('#')) {
    return color;
  }

  // If it's a hex without #, add it
  if (/^[0-9A-Fa-f]{6}$/.test(color)) {
    return `#${color}`;
  }

  return `#${color}`;
};

// Use computed property to get colors to display to ensure reactivity
const colorsToDisplay = computed(() => {
  if (!props.colorEffect) return [];

  // For single color effect
  if (props.colorEffect.type === 'single' && props.colorEffect.color) {
    return [formatColorIfNeeded(props.colorEffect.color)];
  }

  // For rainbow effect, create a simulated rainbow gradient (5 colors)
  if (props.colorEffect.type === 'rainbow') {
    return ['#FF0000', '#FFFF00', '#00FF00', '#00FFFF', '#FF00FF'];
  }

  // For multiple color effects
  if (['fade', 'sections', 'sectionsWave', 'switch'].includes(props.colorEffect.type) &&
      Array.isArray(props.colorEffect.colors)) {
    return props.colorEffect.colors.map(color => formatColorIfNeeded(color));
  }

  return [];
});
</script>

<template>
  <div
    class="relative px-4 py-3 rounded-md cursor-pointer transition-all duration-200 flex items-center gap-3 border"
    :class="isActive
      ? 'border-indigo-400 bg-indigo-50 text-indigo-900 shadow-sm'
      : 'border-gray-300 bg-gray-100 hover:bg-gray-200'"
    @click="handleClick"
    draggable="true"
    @dragstart="handleDragStart"
    @dragend="handleDragEnd"
    @dragover="handleDragOver"
    @drop="handleDrop"
  >
    <!-- Drag handle -->
    <div class="cursor-move text-gray-400 hover:text-gray-600">
      <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="8" cy="6" r="1" />
        <circle cx="8" cy="12" r="1" />
        <circle cx="8" cy="18" r="1" />
        <circle cx="16" cy="6" r="1" />
        <circle cx="16" cy="12" r="1" />
        <circle cx="16" cy="18" r="1" />
      </svg>
    </div>

    <!-- Module name -->
    <span>{{ name }}</span>

    <!-- Color indicators -->
    <div class="flex -space-x-1" v-if="colorEffect">
      <div
        v-for="(color, index) in colorsToDisplay"
        :key="index"
        class="w-4 h-4 rounded-full border border-white"
        :style="{ backgroundColor: color }"
        :title="color"
      ></div>
    </div>

    <!-- Duration dropdown -->
    <div class="flex items-center ml-auto" @click.stop>
      <select
        :value="duration"
        @change="updateDuration"
        class="text-sm border rounded px-2 py-1"
        :class="isActive
          ? 'bg-indigo-100 text-indigo-900 border-indigo-300'
          : 'bg-white text-gray-800 border-gray-300'"
      >
        <option value="500">0.5s</option>
        <option value="1000">1s</option>
        <option value="2000">2s</option>
        <option value="3000">3s</option>
        <option value="5000">5s</option>
        <option value="10000">10s</option>
        <option value="20000">20s</option>
        <option value="30000">30s</option>
      </select>
    </div>

    <!-- Remove button -->
    <button
      class="w-5 h-5 rounded-full flex items-center justify-center text-xs hover:bg-red-600 hover:text-white transition-colors"
      :class="isActive
        ? 'bg-indigo-200 text-indigo-800'
        : 'bg-gray-200 text-gray-600'"
      @click="handleRemove"
    >
      ×
    </button>
  </div>
</template>

<style scoped>
.dragging {
  opacity: 0.5;
  border: 2px dashed #6366F1 !important;
}
</style>
