import { defineStore } from 'pinia';


interface SerialState {
  port: SerialPort | null;
  reader: ReadableStreamDefaultReader<string> | null;
  writer: WritableStreamDefaultWriter<string> | null;
  decoder: TextDecoderStream | null;
  encoder: TextEncoderStream | null;
  receivedData: string;
  isConnected: boolean;
  error: string | null;
}
// Add the generic type to defineStore for type safety
export const useSerialStore = defineStore<'serial', SerialState>('serial', {
  state: (): SerialState => ({
    port: null,
    reader: null,
    writer: null,
    decoder: null,
    encoder: null,
    receivedData: '',
    isConnected: false,
    error: null,
  }),
  actions: {
    async connectSerial() {
      this.error = null; // Clear previous errors
      try {
        // Request access to a serial port
        this.port = await navigator.serial.requestPort();

        // Open the port with the specified baud rate
        await this.port.open({
          baudRate: 115200,
          dataBits: 8,
          stopBits: 1,
          parity: 'none',
          flowControl: 'none'
        });
        console.log('Serial port opened at 115200 baud.');

        // Set up encoding/decoding streams for reading only
        this.decoder = new TextDecoderStream();

        // Pipe readable stream through decoder for text data
        this.port.readable.pipeTo(this.decoder.writable);

        this.reader = this.decoder.readable.getReader();



        this.isConnected = true;
        this.receivedData = ''; // Clear previous data on new connection

        // Start reading from the port
        this.readSerial();

      } catch (error) {
        console.error('Error connecting to serial port:', error);
        this.error = 'Could not connect to serial port. Make sure a device is connected and you grant permission.';
        this.isConnected = false;
        // Clean up any partially set up state on error
        this.disconnectSerial();
      }
    },

    async readSerial() {
      if (!this.port || !this.reader) return;

      while (this.port && this.port.readable && this.isConnected) {
        try {
          const { value, done } = await this.reader.read();
          if (done) {
            console.log('Reader closed.');
            break;
          }
          // Append received data to the state
          this.receivedData += value;
        } catch (error) {
          console.error('Error reading from serial port:', error);
          this.error = 'Error reading from serial port.';
          this.isConnected = false; // Mark as disconnected on read error
          break; // Exit loop on error
        }
      }
      // If the loop exits unexpectedly while still connected (e.g., device unplugged),
      // ensure we call disconnect to clean up.
      if (this.isConnected) {
          this.disconnectSerial();
      }
    },

    async writeSerial(data: Uint8Array) {
      if (!this.port || !this.port.writable) {
        console.error('Serial port not open for writing.');
        this.error = 'Serial port not open for writing.';
        return;
      }

      try {
        const prependedData = new Uint8Array(data.length + 2);
        prependedData[0] = data.length % 256;  // First prepended value (length)
        prependedData[1] = 0; //Math.floor(data.length / 256);       // Second prepended value
        prependedData.set(data, 2); // Copy original data starting at index 2

        console.log("BEFORE WRITE", prependedData);
        const writer = this.port.writable.getWriter();
        await writer.write(prependedData);
        writer.releaseLock();

      } catch (error) {
        console.error('Error writing to serial port:', error);
        this.error = 'Error writing to serial port.';
      }
    },

    async disconnectSerial() {
      if (this.reader) {
        try {
          await this.reader.cancel(); // Cancel any pending read operations
          await this.reader.releaseLock(); // Release the lock
        } catch (error) {
          console.warn('Error cancelling/releasing reader lock:', error);
        }
        this.reader = null;
      }
      if (this.writer) {
        try {
          await this.writer.close(); // Close the writer
          await this.writer.releaseLock(); // Release the lock
        } catch (error) {
          console.warn('Error closing/releasing writer lock:', error);
        }
        this.writer = null;
      }
      if (this.port) {
        try {
          if (this.port.readable.locked) { // Check if readable stream is still locked
             // This might not be necessary if reader.cancel() and releaseLock() handle it
             // but can be a safeguard
          }
          if (this.port.writable.locked) { // Check if writable stream is still locked
             // Similar safeguard
          }
          await this.port.close();
          console.log('Serial port closed.');
        } catch (error) {
          console.error('Error closing serial port:', error);
          this.error = 'Error closing serial port.';
        }
        this.port = null;
      }

      // Reset all state properties
      this.$reset(); // Pinia's built-in way to reset state
      this.isConnected = false; // Ensure isConnected is false after reset
    },
  },
});
