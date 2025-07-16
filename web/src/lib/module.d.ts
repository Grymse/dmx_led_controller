import type { protocol } from "./protobuf/protocol";

export type Module = {
  id: number;
  name: string;
  duration: number;
  direction: protocol.Direction;
  colorEffect: { type: string; [key: string]: any };
  mask1: { type: string; [key: string]: any; sections?: number[] } | null;
  mask2: { type: string; [key: string]: any; sections?: number[] } | null;
}
