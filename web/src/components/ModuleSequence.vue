<script setup lang="ts">
import { ref, computed, defineProps, defineEmits } from 'vue';
import AnimationModule from "@/components/AnimationModule.vue";

// Define the Module interface
interface Module {
  id: number;
  name: string;
  duration: number;
  colorEffect: { type: string; [key: string]: any };
  mask1: { type: string; [key: string]: any };
  mask2: { type: string; [key: string]: any };
}

const props = defineProps<{
  modules: Module[];
  activeModuleId: number | null;
}>();

const emit = defineEmits([
  'update:modules',
  'update:activeModuleId',
  'update:duration',
  'add',
  'remove',
  'select'
]);

// Track drag and drop operations
const isDragging = ref(false);
const draggedIndex = ref<number | null>(null);
const dragOverIndex = ref<number | null>(null);
const dropPosition = ref<'before' | 'after' | null>(null);
const showEndDropIndicator = ref(false);
const animateSwap = ref(false); // Flag to trigger animation

// Reference to the modules container
const modulesContainer = ref<HTMLElement | null>(null);

// Module management functions
const selectModule = (id: number) => {
  emit('select', id);
};

const addModule = () => {
  emit('add');
};

const removeModule = (id: number) => {
  emit('remove', id);
};

// Update module duration
const updateModuleDuration = (id: number, duration: number) => {
  emit('update:duration', id, duration);
};

// Handle container dragover to detect end-of-list drops and start-of-list drops
const handleContainerDragOver = (e: DragEvent) => {
  if (!isDragging.value || !modulesContainer.value) return;

  e.preventDefault();

  // Get container dimensions
  const containerRect = modulesContainer.value.getBoundingClientRect();
  const lastModuleIndex = props.modules.length - 1;

  // If we're at the beginning of the container (left side)
  if (e.clientX < containerRect.left + 50) {
    // Show indicator for dropping at the beginning
    dragOverIndex.value = 0;
    dropPosition.value = 'before';
    showEndDropIndicator.value = false;
  }
  // If we're at the end of the container (right side)
  else if (e.clientX > containerRect.right - 100) {
    showEndDropIndicator.value = true;
    dragOverIndex.value = lastModuleIndex;
    dropPosition.value = 'after';
  }
  // If we're not over any specific module but still in the container
  else if (!dragOverIndex.value) {
    showEndDropIndicator.value = true;
    dragOverIndex.value = lastModuleIndex;
    dropPosition.value = 'after';
  }
};

// Drag and drop handlers
const handleDragStart = (index: number) => {
  isDragging.value = true;
  draggedIndex.value = index;
};

const handleDragOver = ({ index, position }: { index: number, position: 'before' | 'after' }) => {
  // Don't allow dropping on self
  if (index !== draggedIndex.value) {
    dragOverIndex.value = index;
    dropPosition.value = position;
    showEndDropIndicator.value = (index === props.modules.length - 1 && position === 'after');
  } else {
    dragOverIndex.value = null;
    dropPosition.value = null;
    showEndDropIndicator.value = false;
  }
};

const handleContainerDrop = (e: DragEvent) => {
  e.preventDefault();

  if (!isDragging.value || draggedIndex.value === null) return;

  const newModules = [...props.modules];

  // If we're dropping at the beginning
  if (dragOverIndex.value === 0 && dropPosition.value === 'before') {
    const fromIndex = draggedIndex.value;

    // If the dragged module is already the first one, do nothing
    if (fromIndex === 0) {
      resetDragState();
      return;
    }

    // Trigger animation
    animateSwap.value = true;

    // Remove the dragged item
    const moduleToMove = newModules.splice(fromIndex, 1)[0];

    // Add it at the beginning
    newModules.unshift(moduleToMove);

    // Emit updated modules
    emit('update:modules', newModules);

    // Reset animation flag after a short delay
    setTimeout(() => {
      animateSwap.value = false;
    }, 600); // Matches animation duration
  }
  // If we're dropping at the end
  else if (showEndDropIndicator.value) {
    const fromIndex = draggedIndex.value;

    // If the dragged module is already the last one, do nothing
    if (fromIndex === newModules.length - 1) {
      resetDragState();
      return;
    }

    // Trigger animation
    animateSwap.value = true;

    // Remove the dragged item
    const moduleToMove = newModules.splice(fromIndex, 1)[0];

    // Add it at the end
    newModules.push(moduleToMove);

    // Emit updated modules
    emit('update:modules', newModules);

    // Reset animation flag after a short delay
    setTimeout(() => {
      animateSwap.value = false;
    }, 600); // Matches animation duration
  }

  resetDragState();
};

const handleDrop = ({ fromIndex, toIndex, position }: { fromIndex: number, toIndex: number, position: 'before' | 'after' }) => {
  if (fromIndex === toIndex) return;

  const newModules = [...props.modules];

  // Calculate the actual insertion point
  let actualToIndex = toIndex;

  // If dropping after an element and that element is before the dragged element,
  // we need to adjust the index
  if (position === 'after') {
    actualToIndex++;
  }

  // Adjust for the removal of the dragged item
  if (fromIndex < actualToIndex) {
    actualToIndex--;
  }

  // Trigger animation before the swap
  animateSwap.value = true;

  // Remove the dragged item
  const moduleToMove = newModules.splice(fromIndex, 1)[0];

  // Insert it at the new position
  newModules.splice(actualToIndex, 0, moduleToMove);

  // Emit the updated modules
  emit('update:modules', newModules);

  // Reset animation flag after a short delay
  setTimeout(() => {
    animateSwap.value = false;
  }, 600); // Matches animation duration

  // Reset tracking variables
  resetDragState();
};

// Handle drag end event - reset all drag-related state
const handleDragEnd = () => {
  resetDragState();
};

const resetDragState = () => {
  isDragging.value = false;
  draggedIndex.value = null;
  dragOverIndex.value = null;
  dropPosition.value = null;
  showEndDropIndicator.value = false;
};

// Calculate total animation duration
const totalDuration = computed(() => {
  return props.modules.reduce((total, module) => total + module.duration, 0);
});
</script>

<template>
  <div class="w-full py-4 bg-gray-50 border-b border-gray-200">
    <div class="px-6">
      <div class="flex items-center justify-between mb-3">
        <div class="text-lg text-gray-600">
          Program
        </div>
        <div class="text-sm text-gray-600">
          Total Duration: {{ (totalDuration / 1000).toFixed(1) }}s
        </div>
      </div>

      <!-- Animation Modules Container with Drag Event Handling -->
      <div
        ref="modulesContainer"
        class="modules-container flex flex-wrap items-center"
        @dragend="handleDragEnd"
        @dragover="handleContainerDragOver"
        @drop="handleContainerDrop"
        :class="{ 'animating-modules': animateSwap }"
      >
        <!-- Start drop indicator -->
        <div
          v-if="isDragging && dragOverIndex === 0 && dropPosition === 'before'"
          class="start-drop-indicator"
        ></div>

        <transition-group name="module-swap">
          <AnimationModule
            v-for="(module, index) in modules"
            :key="`module-${module.id}`"
            :id="module.id"
            :name="module.name"
            :duration="module.duration"
            :is-active="module.id === activeModuleId"
            :color-effect="module.colorEffect"
            :index="index"
            :is-dragging="isDragging"
            :drag-over-index="dragOverIndex"
            :drop-position="dropPosition"
            :dragged-index="draggedIndex"
            class="module-item"
            @click="selectModule"
            @remove="removeModule"
            @update:duration="updateModuleDuration"
            @dragstart="handleDragStart"
            @dragover="handleDragOver"
            @drop="handleDrop"
          />
        </transition-group>

        <!-- Add Module Button -->
        <button
          @click="addModule"
          class="w-8 h-8 bg-gray-200 hover:bg-gray-300 rounded-full flex items-center justify-center text-xl font-bold transition-colors ml-2"
        >
          +
        </button>

        <!-- End drop indicator -->
        <div
          v-if="showEndDropIndicator"
          class="end-drop-indicator"
        ></div>
      </div>
    </div>
  </div>
</template>

<style scoped>
.modules-container {
  position: relative;
  min-height: 60px;
  padding: 8px 0;
}

.end-drop-indicator {
  position: relative;
  width: 4px;
  height: 30px;
  background-color: #6366F1;
  border-radius: 2px;
  margin-left: 8px;
  animation: pulse 1.5s infinite;
}

.start-drop-indicator {
  position: relative;
  width: 4px;
  height: 30px;
  background-color: #6366F1;
  border-radius: 2px;
  margin-right: 8px;
  animation: pulse 1.5s infinite;
}

/* Animation for swapping modules */
.module-swap-move {
  transition: transform 0.5s ease;
}

.module-swap-enter-active,
.module-swap-leave-active {
  transition: all 0.5s ease;
}

.module-swap-enter-from {
  opacity: 0;
  transform: translateY(30px);
}

.module-swap-leave-to {
  opacity: 0;
  transform: translateX(-30px);
  position: absolute;
}

/* Apply a brief highlight animation to modules when they're rearranged */
.animating-modules .module-item {
  animation: module-highlight 0.6s ease;
}

@keyframes module-highlight {
  0% {
    box-shadow: 0 0 0 2px rgba(99, 102, 241, 0);
  }
  20% {
    box-shadow: 0 0 0 2px rgba(99, 102, 241, 0.8);
  }
  100% {
    box-shadow: 0 0 0 2px rgba(99, 102, 241, 0);
  }
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
