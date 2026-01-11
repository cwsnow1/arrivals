#include <stdint.h>
#include <stddef.h>

// In a separate file bc intellisense doesn't recognize #embed and it was annoying mes

uint8_t index_html[] = {
#embed "index.html.gz" // run minify.py first TODO: make CMake run this automatically
};
size_t index_html_size = sizeof index_html;
