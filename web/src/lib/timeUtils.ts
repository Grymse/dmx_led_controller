/**
 * Utility functions for handling time conversions between ticks and milliseconds
 * In this application, 1 tick = 1/40th of a second = 25ms
 */

// Constants
export const TICKS_PER_SECOND = 40;
export const MS_PER_TICK = 25; // 1000ms / 40 ticks

/**
 * Convert milliseconds to ticks
 * @param ms Time in milliseconds
 * @returns Equivalent number of ticks (rounded to nearest tick)
 */
export function msToTicks(ms: number): number {
  return Math.round(ms / MS_PER_TICK);
}

/**
 * Convert ticks to milliseconds
 * @param ticks Number of ticks
 * @returns Equivalent time in milliseconds
 */
export function ticksToMs(ticks: number): number {
  return ticks * MS_PER_TICK;
}

/**
 * Convert a parameter between ticks and milliseconds based on its type
 * @param value The parameter value
 * @param paramKey The parameter key/name
 * @param toTicks Direction of conversion (true = to ticks, false = to ms)
 * @returns Converted value
 */
export function convertTimeParameter(value: number, paramKey: string, toTicks: boolean): number {
  // List of parameters that represent time durations
  const timeParameters = ['duration'];

  // Only convert time parameters
  if (timeParameters.includes(paramKey)) {
    return toTicks ? msToTicks(value) : ticksToMs(value);
  }

  // Return unchanged for non-time parameters
  return value;
}

/**
 * Process an effect/mask object to convert time parameters
 * @param obj Object containing parameters
 * @param toTicks Direction of conversion (true = to ticks, false = to ms)
 * @returns New object with converted values
 */
export function convertTimeValues(obj: Record<string, any>, toTicks: boolean): Record<string, any> {
  const result: Record<string, any> = { ...obj };

  // Process each parameter in the object
  Object.keys(obj).forEach(key => {
    if (typeof obj[key] === 'number') {
      result[key] = convertTimeParameter(obj[key], key, toTicks);
    }
  });

  return result;
}

/**
 * Convert a hex color string to uint32 representation (0xRRGGBB)
 * @param hexColor Hex color string (with or without # prefix)
 * @returns Color as uint32 number
 */
export function hexColorToUint32(hexColor: string): number {
  // Remove # if present
  hexColor = hexColor.replace('#', '');

  // Parse as RGB
  const r = parseInt(hexColor.substring(0, 2), 16);
  const g = parseInt(hexColor.substring(2, 4), 16);
  const b = parseInt(hexColor.substring(4, 6), 16);

  // Pack into uint32 (0xRRGGBB)
  return (r << 16) | (g << 8) | b;
}
