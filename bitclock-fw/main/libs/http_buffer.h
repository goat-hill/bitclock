//
// The full NOAA response is ~17kB but we only look at the the first couple of
// time periods Expected length is ~4kB
//
#define MAX_HTTP_OUTPUT_BUFFER 1024 * 24
extern char http_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1];
