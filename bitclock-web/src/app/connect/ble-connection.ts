interface BluetoothConnection {
  device: BluetoothDevice;
  server: BluetoothRemoteGATTServer;
  service: BluetoothRemoteGATTService;
}

export type { BluetoothConnection };
