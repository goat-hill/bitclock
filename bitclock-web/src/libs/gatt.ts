export const SERVICE_UUID = "ec2f3aa0-4ed8-71bd-f147-b2bc37195232";

export const CHR_WIFI_UUID = "a5313036-a66e-e990-2147-49a7b1557044";
export const CHR_WIFI_STATUS_UUID = "32f4b3c3-d9b2-4d26-8da6-c9e88711b65d";

export const CHR_OTA_SIZE_UUID = "c839db31-c8e7-4c59-938d-c4b46e7bc2a4";
export const CHR_OTA_DATA_UUID = "ed900bcc-2ed5-40b0-b33a-fe7483f254c7";

export const CHR_WEATHER_PATH_UUID = "32efee1d-9566-47e2-b880-0026d48c1f81";
export const CHR_MQTT_URL_UUID = "2ab48c15-934d-11ec-b90a-802748c27b9e";

export const CHR_TEMPERATURE_UUID = 0x2a6e;
export const CHR_HUMIDITY_UUID = 0x2a6f;
export const CHR_CO2_UUID = 0x2b8c;
export const CHR_VOC_INDEX_UUID = 0x2be7;
export const CHR_NOX_INDEX_UUID = 0x2bd2;
export const CHR_TEMPERATURE_UNIT_UUID = 0x2a1d;

export const CHR_TIMEZONE_UUID = 0x2a0e;
export const CHR_CLOCK_FORMAT_UUID = "8323a538-871d-4465-a0e5-2a8759c445d6";

export const TEMP_UNIT_VAL_CELSIUS = 1;
export const TEMP_UNIT_VAL_FAHRENHEIT = 2;

export const CLOCK_FORMAT_VAL_12HR = 1;
export const CLOCK_FORMAT_VAL_24HR = 2;

export const CHR_APP_SELECTION_UUID = "3504c5e2-0aee-43eb-9aa2-f77346e249ea";
export const APP_SELECTION_VAL_CLOCK = 1;
export const APP_SELECTION_VAL_WEATHER = 2;

// let CHR_DEVICE_NAME_UUID = 0x2a00;
// let CHR_FW_REV_UUID = 0x2a26;
// let CHR_HW_REV_UUID = 0x2a27;
// let CHR_LANGUAGE_UUID = 0x2aa2;

export type WifiStatus = {
	isStarted: boolean;
	isConnected: boolean;
};
export const WIFI_IS_STARTED_VAL_BIT = 1;
export const WIFI_IS_CONNECTED_VAL_BIT = 1 << 1;

export const parseWifiStatus = (value: DataView): WifiStatus => {
	const status = value.getUint8(0);
	return {
		isStarted: (status & WIFI_IS_STARTED_VAL_BIT) !== 0,
		isConnected: (status & WIFI_IS_CONNECTED_VAL_BIT) !== 0,
	};
};


