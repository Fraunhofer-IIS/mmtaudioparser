# Main Page {#mainpage}

## Description

The mmtaudioparser is a utility library to parse configuration structures of supported compressed audio formats. Currently, only MPEG-H 3D Audio (defined in ISO/IEC 23008-3) is supported.

The library (via the [IAudioParser](@ref mmt::audioparser::IAudioParser) interface) consumes raw configuration packets, checks them for validity and produces a structured codec-specific representation of the binary configuration data.

## Minimal Code Example

The following lines provide a simple example showing how to parse MPEG-H 3D Audio configuration packets.

```{.c}
#include "mmtaudioparser/mpeghparser.h"

using namespace mmt;
using namespace mmt::audioparser;

...

ilo::ByteBuffer mpegh3daConfigPacket = ...;

CMpeghParser parser{};
parser.addConfig(mpegh3daConfigPacket);
if (parser.isValidConfig()) {
  SConfigInfo config = parser.getConfigInfo();
  // do something with the config structure
}
```
