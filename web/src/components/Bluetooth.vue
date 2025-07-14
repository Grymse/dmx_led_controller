<script setup lang="ts">
import { ref, computed } from 'vue'
import { useBleStore } from '../stores/bluetooth'

const bleStore = useBleStore()

// Component-specific reactive data
const messageToSend = ref('')
const messageFilter = ref<'all' | 'received' | 'sent' | 'system'>('all')

// Computed properties
const connectionStatusClass = computed(() => ({
  'status-connected': bleStore.isConnected,
  'status-connecting': bleStore.connectionStatus === 'connecting',
  'status-disconnected': bleStore.connectionStatus === 'disconnected',
  'status-error': bleStore.connectionStatus === 'error'
}))

const filteredMessages = computed(() => {
  if (messageFilter.value === 'all') {
    return bleStore.messages
  }
  return bleStore.messages.filter(msg => msg.type === messageFilter.value)
})

// Methods
const connect = async (): Promise<void> => {
  await bleStore.connect()
}

const disconnect = async (): Promise<void> => {
  await bleStore.disconnect()
}

const sendMessage = async (): Promise<void> => {
  if (!messageToSend.value.trim()) return

  const success = await bleStore.sendData(messageToSend.value)
  if (success) {
    messageToSend.value = ''
  }
}

const sendJsonMessage = async (): Promise<void> => {
  if (!messageToSend.value.trim()) return

  try {
    const jsonData = JSON.parse(messageToSend.value)
    const success = await bleStore.sendJson(jsonData)
    if (success) {
      messageToSend.value = ''
    }
  } catch (error) {
    bleStore.addMessage('Invalid JSON format', 'system')
  }
}

const sendQuickMessage = async (message: string): Promise<void> => {
  await bleStore.sendData(message)
}

const sendTestPayload = async (): Promise<void> => {
  // Generate a test payload close to 300 bytes
  const testData = {
    timestamp: new Date().toISOString(),
    sensor_data: {
      temperature: 25.6,
      humidity: 60.2,
      pressure: 1013.25,
      light: 450,
      motion: false
    },
    device_info: {
      id: 'ESP32-C3-001',
      firmware: '1.0.0',
      uptime: 123456
    },
    payload: 'A'.repeat(200) // Padding to increase size
  }

  await bleStore.sendJson(testData)
}

// Keyboard shortcuts
const handleKeydown = (event: KeyboardEvent): void => {
  if (event.ctrlKey && event.key === 'Enter') {
    sendMessage()
  }
}

// Add event listener for keyboard shortcuts
document.addEventListener('keydown', handleKeydown)
</script>

<template>
  <div class="ble-client">
    <h2>ESP32-C3 BLE Client</h2>

    <div class="connection-status">
      <p>Status: <span :class="connectionStatusClass">{{ bleStore.connectionStatusText }}</span></p>
      <p v-if="bleStore.deviceName">Device: <strong>{{ bleStore.deviceName }}</strong></p>
      <div v-if="bleStore.lastError" class="error-message">
        <p>{{ bleStore.lastError }}</p>
        <button @click="bleStore.clearError" class="btn btn-sm">Clear Error</button>
      </div>
    </div>

    <div class="controls">
      <button @click="connect" :disabled="bleStore.isConnected" class="btn btn-primary">
        Connect to ESP32
      </button>
      <button @click="disconnect" :disabled="!bleStore.isConnected" class="btn btn-secondary">
        Disconnect
      </button>
      <button @click="bleStore.clearMessages" class="btn btn-outline">
        Clear Messages
      </button>
    </div>

    <div class="advanced-controls">
      <label class="checkbox-label">
        <input
          type="checkbox"
          :checked="bleStore.enableAutoReconnect"
          @change="bleStore.setAutoReconnect($event.target.checked)"
        />
        Auto-reconnect
      </label>
    </div>

    <div v-if="bleStore.isConnected" class="data-section">
      <div class="send-data">
        <h3>Send Data</h3>
        <div class="input-group">
          <textarea
            v-model="messageToSend"
            placeholder="Enter message to send..."
            rows="3"
            class="message-input"
            @keydown.ctrl.enter="sendMessage"
          ></textarea>
          <div class="send-buttons">
            <button @click="sendMessage" :disabled="!messageToSend.trim()" class="btn btn-success">
              Send Text
            </button>
            <button @click="sendJsonMessage" :disabled="!messageToSend.trim()" class="btn btn-info">
              Send as JSON
            </button>
          </div>
        </div>

        <div class="quick-actions">
          <h4>Quick Actions</h4>
          <button @click="sendQuickMessage('ping')" class="btn btn-sm">Ping</button>
          <button @click="sendQuickMessage('status')" class="btn btn-sm">Status</button>
          <button @click="sendTestPayload" class="btn btn-sm">Test Large Payload</button>
        </div>
      </div>

      <div class="received-data">
        <h3>Messages ({{ bleStore.messages.length }})</h3>
        <div class="message-filters">
          <label>
            <input type="radio" v-model="messageFilter" value="all" />
            All
          </label>
          <label>
            <input type="radio" v-model="messageFilter" value="received" />
            Received
          </label>
          <label>
            <input type="radio" v-model="messageFilter" value="sent" />
            Sent
          </label>
          <label>
            <input type="radio" v-model="messageFilter" value="system" />
            System
          </label>
        </div>

        <div class="data-log">
          <div
            v-for="(message, index) in filteredMessages"
            :key="index"
            :class="['message', `message-${message.type}`]"
          >
            <span class="timestamp">{{ message.timestamp }}</span>
            <span class="message-type">{{ message.type.toUpperCase() }}</span>
            <span class="content">{{ message.content }}</span>
          </div>
          <div v-if="filteredMessages.length === 0" class="no-messages">
            No messages to display
          </div>
        </div>
      </div>
    </div>

    <div v-if="!bleStore.isWebBluetoothSupported()" class="browser-warning">
      <p><strong>Warning:</strong> Web Bluetooth is not supported in this browser.</p>
      <p>Please use Chrome, Edge, or a compatible browser.</p>
    </div>
  </div>
</template>

<style scoped>
.ble-client {
  max-width: 1000px;
  margin: 0 auto;
  padding: 20px;
}

.connection-status {
  margin-bottom: 20px;
}

.status-connected {
  color: #28a745;
  font-weight: bold;
}

.status-connecting {
  color: #ffc107;
  font-weight: bold;
}

.status-disconnected {
  color: #6c757d;
  font-weight: bold;
}

.status-error {
  color: #dc3545;
  font-weight: bold;
}

.error-message {
  background-color: #f8d7da;
  border: 1px solid #f5c6cb;
  border-radius: 5px;
  padding: 10px;
  margin-top: 10px;
}

.error-message p {
  margin: 0 0 10px 0;
  color: #721c24;
}

.controls {
  margin-bottom: 20px;
}

.advanced-controls {
  margin-bottom: 20px;
}

.checkbox-label {
  display: flex;
  align-items: center;
  gap: 8px;
  cursor: pointer;
}

.btn {
  padding: 8px 16px;
  margin-right: 10px;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  font-size: 14px;
  transition: background-color 0.2s;
}

.btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.btn-primary {
  background-color: #007bff;
  color: white;
}

.btn-secondary {
  background-color: #6c757d;
  color: white;
}

.btn-success {
  background-color: #28a745;
  color: white;
}

.btn-info {
  background-color: #17a2b8;
  color: white;
}

.btn-outline {
  background-color: transparent;
  border: 1px solid #6c757d;
  color: #6c757d;
}

.btn-sm {
  padding: 4px 8px;
  font-size: 12px;
}

.data-section {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 30px;
}

.input-group {
  margin-bottom: 20px;
}

.message-input {
  width: 100%;
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 5px;
  margin-bottom: 10px;
  font-family: monospace;
  resize: vertical;
}

.send-buttons {
  display: flex;
  gap: 10px;
}

.quick-actions {
  margin-top: 20px;
}

.quick-actions h4 {
  margin-bottom: 10px;
}

.message-filters {
  display: flex;
  gap: 15px;
  margin-bottom: 10px;
}

.message-filters label {
  display: flex;
  align-items: center;
  gap: 5px;
  cursor: pointer;
}

.data-log {
  height: 400px;
  overflow-y: auto;
  border: 1px solid #ddd;
  border-radius: 5px;
  padding: 10px;
  background-color: #f8f9fa;
}

.message {
  margin-bottom: 8px;
  padding: 8px;
  border-radius: 4px;
  background-color: white;
  border-left: 3px solid #007bff;
}

.message-received {
  border-left-color: #28a745;
}

.message-sent {
  border-left-color: #17a2b8;
}

.message-system {
  border-left-color: #ffc107;
  background-color: #fff3cd;
}

.timestamp {
  font-size: 11px;
  color: #666;
  margin-right: 8px;
}

.message-type {
  font-size: 10px;
  color: #666;
  margin-right: 8px;
  font-weight: bold;
}

.content {
  font-family: monospace;
  word-break: break-word;
}

.no-messages {
  text-align: center;
  color: #6c757d;
  padding: 40px;
}

.browser-warning {
  background-color: #f8d7da;
  border: 1px solid #f5c6cb;
  border-radius: 5px;
  padding: 15px;
  margin-top: 20px;
}

.browser-warning p {
  margin: 0;
  color: #721c24;
}

@media (max-width: 768px) {
  .data-section {
    grid-template-columns: 1fr;
  }

  .send-buttons {
    flex-direction: column;
  }

  .message-filters {
    flex-wrap: wrap;
  }
}
</style>
