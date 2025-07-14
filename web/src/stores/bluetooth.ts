import { defineStore } from 'pinia'
import { ref, computed } from 'vue'

// BLE Service and Characteristic UUIDs (must match ESP32)
const SERVICE_UUID = '12345678-1234-1234-1234-123456789abc'
const CHARACTERISTIC_UUID_RX = '12345678-1234-1234-1234-123456789abd'
const CHARACTERISTIC_UUID_TX = '12345678-1234-1234-1234-123456789abe'

export interface BleMessage {
  timestamp: string
  content: string
  type: 'sent' | 'received' | 'system'
}

export const useBleStore = defineStore('ble', () => {
  // State
  const isConnected = ref(false)
  const connectionStatus = ref<'disconnected' | 'connecting' | 'connected' | 'error'>('disconnected')
  const deviceName = ref<string>('')
  const messages = ref<BleMessage[]>([])
  const lastError = ref<string>('')

  // BLE objects
  let bleDevice: BluetoothDevice | null = null
  let bleServer: BluetoothRemoteGATTServer | null = null
  let bleService: BluetoothRemoteGATTService | null = null
  let rxCharacteristic: BluetoothRemoteGATTCharacteristic | null = null
  let txCharacteristic: BluetoothRemoteGATTCharacteristic | null = null

  // Computed
  const connectionStatusText = computed(() => {
    switch (connectionStatus.value) {
      case 'disconnected':
        return 'Disconnected'
      case 'connecting':
        return 'Connecting...'
      case 'connected':
        return 'Connected'
      case 'error':
        return 'Connection Error'
      default:
        return 'Unknown'
    }
  })

  const receivedMessages = computed(() =>
    messages.value.filter(msg => msg.type === 'received')
  )

  const sentMessages = computed(() =>
    messages.value.filter(msg => msg.type === 'sent')
  )

  // Actions
  const isWebBluetoothSupported = (): boolean => {
    return navigator.bluetooth !== undefined
  }

  const addMessage = (content: string, type: BleMessage['type'] = 'system'): void => {
    const timestamp = new Date().toLocaleTimeString()
    messages.value.unshift({ timestamp, content, type })

    // Keep only last 100 messages
    if (messages.value.length > 100) {
      messages.value = messages.value.slice(0, 100)
    }
  }

  const setError = (error: string): void => {
    lastError.value = error
    connectionStatus.value = 'error'
    addMessage(`Error: ${error}`, 'system')
  }

  const handleNotification = (event: Event): void => {
    const target = event.target as BluetoothRemoteGATTCharacteristic
    const decoder = new TextDecoder()
    const value = decoder.decode(target.value!)

    addMessage(value, 'received')
  }

  const handleDisconnection = (): void => {
    isConnected.value = false
    connectionStatus.value = 'disconnected'
    deviceName.value = ''

    // Clean up references
    bleDevice = null
    bleServer = null
    bleService = null
    rxCharacteristic = null
    txCharacteristic = null

    addMessage('Disconnected from ESP32-C3', 'system')
  }

  const connect = async (deviceNameFilter: string = 'ESP32-C3-BLE'): Promise<boolean> => {
    if (!isWebBluetoothSupported()) {
      setError('Web Bluetooth is not supported in this browser')
      return false
    }

    try {
      connectionStatus.value = 'connecting'
      lastError.value = ''

      // Request device
      bleDevice = await navigator.bluetooth.requestDevice({
        filters: [{
          name: deviceNameFilter
        }],
        optionalServices: [SERVICE_UUID]
      })

      deviceName.value = bleDevice.name || 'Unknown Device'

      // Connect to GATT server
      bleServer = await bleDevice.gatt!.connect()

      // Get service
      bleService = await bleServer.getPrimaryService(SERVICE_UUID)

      // Get characteristics
      rxCharacteristic = await bleService.getCharacteristic(CHARACTERISTIC_UUID_RX)
      txCharacteristic = await bleService.getCharacteristic(CHARACTERISTIC_UUID_TX)

      // Subscribe to notifications
      await txCharacteristic.startNotifications()
      txCharacteristic.addEventListener('characteristicvaluechanged', handleNotification)

      // Set up disconnect handler
      bleDevice.addEventListener('gattserverdisconnected', handleDisconnection)

      isConnected.value = true
      connectionStatus.value = 'connected'

      addMessage(`Connected to ${deviceName.value}`, 'system')

      return true

    } catch (error) {
      const errorMessage = error instanceof Error ? error.message : 'Unknown connection error'
      setError(`Connection failed: ${errorMessage}`)
      return false
    }
  }

  const disconnect = async (): Promise<void> => {
    if (bleDevice && bleDevice.gatt!.connected) {
      try {
        await bleDevice.gatt!.disconnect()
      } catch (error) {
        console.error('Disconnect error:', error)
      }
    }
    handleDisconnection()
  }

  const sendData = async (data: string): Promise<boolean> => {
    if (!rxCharacteristic || !data.trim()) {
      setError('Cannot send data: not connected or empty message')
      return false
    }

    try {
      const encoder = new TextEncoder()
      const encodedData = encoder.encode(data)

      // Check if data is too large for single packet
      if (encodedData.length > 512) {
        setError('Message too large. Maximum size is 512 bytes.')
        return false
      }

      await rxCharacteristic.writeValue(encodedData)
      addMessage(data, 'sent')

      return true

    } catch (error) {
      const errorMessage = error instanceof Error ? error.message : 'Unknown send error'
      setError(`Send failed: ${errorMessage}`)
      return false
    }
  }

  const sendBinaryData = async (data: Uint8Array): Promise<boolean> => {
    if (!rxCharacteristic) {
      setError('Cannot send data: not connected')
      return false
    }

    try {
      // Check if data is too large for single packet
      if (data.length > 512) {
        setError('Data too large. Maximum size is 512 bytes.')
        return false
      }

      await rxCharacteristic.writeValue(data)
      addMessage(`Binary data sent (${data.length} bytes)`, 'sent')

      return true

    } catch (error) {
      const errorMessage = error instanceof Error ? error.message : 'Unknown send error'
      setError(`Send failed: ${errorMessage}`)
      return false
    }
  }

  const sendJson = async (obj: any): Promise<boolean> => {
    try {
      const jsonString = JSON.stringify(obj)
      return await sendData(jsonString)
    } catch (error) {
      setError('Failed to serialize JSON data')
      return false
    }
  }

  const clearMessages = (): void => {
    messages.value = []
  }

  const clearError = (): void => {
    lastError.value = ''
    if (connectionStatus.value === 'error') {
      connectionStatus.value = 'disconnected'
    }
  }

  // Auto-reconnect functionality
  const enableAutoReconnect = ref(false)
  let reconnectAttempts = 0
  const maxReconnectAttempts = 5
  let reconnectTimer: NodeJS.Timeout | null = null

  const attemptReconnect = async (): Promise<void> => {
    if (!enableAutoReconnect.value || reconnectAttempts >= maxReconnectAttempts) {
      return
    }

    reconnectAttempts++
    addMessage(`Attempting to reconnect... (${reconnectAttempts}/${maxReconnectAttempts})`, 'system')

    const success = await connect()
    if (success) {
      reconnectAttempts = 0
      addMessage('Reconnected successfully', 'system')
    } else {
      // Try again in 5 seconds
      reconnectTimer = setTimeout(attemptReconnect, 5000)
    }
  }

  const setAutoReconnect = (enabled: boolean): void => {
    enableAutoReconnect.value = enabled
    if (!enabled && reconnectTimer) {
      clearTimeout(reconnectTimer)
      reconnectTimer = null
      reconnectAttempts = 0
    }
  }

  // Cleanup on disconnect
  const originalHandleDisconnection = handleDisconnection
  const handleDisconnectionWithReconnect = (): void => {
    originalHandleDisconnection()

    if (enableAutoReconnect.value) {
      reconnectTimer = setTimeout(attemptReconnect, 2000)
    }
  }

  return {
    // State
    isConnected,
    connectionStatus,
    connectionStatusText,
    deviceName,
    messages,
    receivedMessages,
    sentMessages,
    lastError,
    enableAutoReconnect,

    // Actions
    connect,
    disconnect,
    sendData,
    sendBinaryData,
    sendJson,
    clearMessages,
    clearError,
    setAutoReconnect,
    isWebBluetoothSupported,
    addMessage
  }
})
