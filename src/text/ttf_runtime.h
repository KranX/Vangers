#ifndef __TEXT_TTF_RUNTIME_H__
#define __TEXT_TTF_RUNTIME_H__

namespace text
{

bool init_ttf_runtime(void);
void shutdown_ttf_runtime(void);
bool ttf_runtime_ready(void);
const char* ttf_runtime_error(void);

}

#endif
