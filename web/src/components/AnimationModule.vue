<script setup lang="ts">
import { ref, defineProps, defineEmits, watch, computed } from 'vue';

const props = defineProps<{
  id: number;
  name: string;
  isActive: boolean;
  duration: number;
  colorEffect?: { type: string; [key: string]: any };
  index: number; // Add index prop to know position
  isDragging: boolean; // Whether any module is being dragged
  dragOverIndex: number | null; // Index being dragged over
  dropPosition: 'before' | 'after' | null; // Position relative to the module
  draggedIndex: number | null; // Index of the module being dragged
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
const isMoving = ref(false);

// Update local duration when prop changes
watch(() => props.duration, (newDuration) => {
  localDuration.value = newDuration;
});

// Track when this module is the drop target
watch(() => props.dragOverIndex, (newIndex) => {
  if (newIndex === props.index) {
    isMoving.value = true;

    // Reset the flag after animation completes
    setTimeout(() => {
      isMoving.value = false;
    }, 300);
  }
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

  // Calculate if we're in the first or second half of the element
  if (e.currentTarget instanceof HTMLElement) {
    const rect = e.currentTarget.getBoundingClientRect();
    const midPoint = rect.left + rect.width / 2;
    const position = e.clientX < midPoint ? 'before' : 'after';

    emit('dragover', { index: props.index, position });
  } else {
    emit('dragover', { index: props.index, position: 'after' });
  }
};

const handleDrop = (e: DragEvent) => {
  e.preventDefault();
  if (e.dataTransfer) {
    const fromIndex = parseInt(e.dataTransfer.getData('text/plain'));

    // Calculate if we're in the first or second half of the element
    if (e.currentTarget instanceof HTMLElement) {
      const rect = e.currentTarget.getBoundingClientRect();
      const midPoint = rect.left + rect.width / 2;
      const position = e.clientX < midPoint ? 'before' : 'after';

      emit('drop', { fromIndex, toIndex: props.index, position });
    } else {
      emit('drop', { fromIndex, toIndex: props.index, position: 'after' });
    }
  }
};

// Determine if this module should show a drop indicator
const showDropIndicator = computed(() => {
  if (!props.isDragging || props.draggedIndex === props.index) return false;

  return props.dragOverIndex === props.index;
});

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
  <div class="module-container" :class="{ 'is-moving': isMoving }">
    <!-- Left drop indicator - only show if dropping BEFORE this module -->
    <div
      v-if="showDropIndicator && dropPosition === 'before'"
      class="drop-indicator"
    ></div>

    <div
      class="relative px-4 py-3 rounded-md cursor-pointer transition-all duration-200 flex items-center gap-3 border module-content"
      :class="[
        isActive
          ? 'border-indigo-400 bg-indigo-50 text-indigo-900 shadow-sm'
          : 'border-gray-300 bg-gray-100 hover:bg-gray-200',
        { 'being-dragged': isDragging && draggedIndex === index }
      ]"
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
          <option value="500">0.25s</option>
          <option value="500">0.5s</option>
          <option value="500">0.75s</option>
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
        class="w-5 h-5 rounded-full flex items-center justify-center text-xs  transition-colors"
        :class="isActive
          ? 'bg-indigo-200 text-indigo-800 hover:bg-indigo-300'
          : 'bg-gray-200 text-gray-600 hover:bg-gray-300'"
        @click="handleRemove"
      >
        ×
      </button>
    </div>

    <!-- Right drop indicator - only show if dropping AFTER this module -->
    <div
      v-if="showDropIndicator && dropPosition === 'after'"
      class="drop-indicator drop-indicator-right"
    ></div>
  </div>
</template>

<style scoped>
.module-container {
  position: relative;
  display: inline-flex;
  align-items: center;
  margin: 0.25rem;
  transition: transform 0.3s ease, opacity 0.3s ease;
}

.being-dragged {
  opacity: 0.5;
}

/* Animation when module is moving to a new position */
.is-moving .module-content {
  transform: scale(1.05);
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
  z-index: 2;
  transition: all 0.3s ease;
}

.drop-indicator {
  position: absolute;
  width: 4px;
  height: 80%;
  background-color: #6366F1;
  border-radius: 2px;
  z-index: 5;
  animation: pulse 1.5s infinite;
  left: 0;
  transform: translateX(-50%);
}

.drop-indicator-right {
  left: auto;
  right: 0;
  transform: translateX(50%);
}

/* Animation when modules are reordered */
@keyframes wiggle {
  0%, 100% { transform: translateX(0); }
  25% { transform: translateX(-3px); }
  75% { transform: translateX(3px); }
}

@keyframes pulse {
  0% {
    box-shadow: 0 0 0 0 rgba(99, 102, 241, 0.7);
  }
  70% {
    box-shadow: 0 0 0 4px rgba(99, 102, 241, 0);
  }
  100% {
    box-shadow: 0 0 0 0 rgba(99, 102, 241, 0);
  }
}
</style>
