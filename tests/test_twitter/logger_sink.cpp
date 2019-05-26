
#include "logger_sink.h"


static std::ostringstream oss;

std::ostringstream& get_ostream_sink() {    
    return oss;
}
