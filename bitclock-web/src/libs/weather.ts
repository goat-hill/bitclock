// NOAA api for geocoding lat/lng to forecast gridpoint

const getNoaaForecastUrl = async (
  lat: string,
  lng: string,
): Promise<string> => {
  const url = `https://api.weather.gov/points/${lat},${lng}`;
  const response = await fetch(url);
  const data = await response.json();
  return data.properties.forecast;
};

export { getNoaaForecastUrl };
