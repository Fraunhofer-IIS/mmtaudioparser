/*-----------------------------------------------------------------------------
Software License for The Fraunhofer FDK MPEG-H Software

Copyright (c) 2019 - 2024 Fraunhofer-Gesellschaft zur Förderung der angewandten
Forschung e.V. and Contributors
All rights reserved.

1. INTRODUCTION

The "Fraunhofer FDK MPEG-H Software" is software that implements the ISO/MPEG
MPEG-H 3D Audio standard for digital audio or related system features. Patent
licenses for necessary patent claims for the Fraunhofer FDK MPEG-H Software
(including those of Fraunhofer), for the use in commercial products and
services, may be obtained from the respective patent owners individually and/or
from Via LA (www.via-la.com).

Fraunhofer supports the development of MPEG-H products and services by offering
additional software, documentation, and technical advice. In addition, it
operates the MPEG-H Trademark Program to ease interoperability testing of end-
products. Please visit www.mpegh.com for more information.

2. COPYRIGHT LICENSE

Redistribution and use in source and binary forms, with or without modification,
are permitted without payment of copyright license fees provided that you
satisfy the following conditions:

* You must retain the complete text of this software license in redistributions
of the Fraunhofer FDK MPEG-H Software or your modifications thereto in source
code form.

* You must retain the complete text of this software license in the
documentation and/or other materials provided with redistributions of
the Fraunhofer FDK MPEG-H Software or your modifications thereto in binary form.
You must make available free of charge copies of the complete source code of
the Fraunhofer FDK MPEG-H Software and your modifications thereto to recipients
of copies in binary form.

* The name of Fraunhofer may not be used to endorse or promote products derived
from the Fraunhofer FDK MPEG-H Software without prior written permission.

* You may not charge copyright license fees for anyone to use, copy or
distribute the Fraunhofer FDK MPEG-H Software or your modifications thereto.

* Your modified versions of the Fraunhofer FDK MPEG-H Software must carry
prominent notices stating that you changed the software and the date of any
change. For modified versions of the Fraunhofer FDK MPEG-H Software, the term
"Fraunhofer FDK MPEG-H Software" must be replaced by the term "Third-Party
Modified Version of the Fraunhofer FDK MPEG-H Software".

3. No PATENT LICENSE

NO EXPRESS OR IMPLIED LICENSES TO ANY PATENT CLAIMS, including without
limitation the patents of Fraunhofer, ARE GRANTED BY THIS SOFTWARE LICENSE.
Fraunhofer provides no warranty of patent non-infringement with respect to this
software. You may use this Fraunhofer FDK MPEG-H Software or modifications
thereto only for purposes that are authorized by appropriate patent licenses.

4. DISCLAIMER

This Fraunhofer FDK MPEG-H Software is provided by Fraunhofer on behalf of the
copyright holders and contributors "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED
WARRANTIES, including but not limited to the implied warranties of
merchantability and fitness for a particular purpose. IN NO EVENT SHALL THE
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE for any direct, indirect,
incidental, special, exemplary, or consequential damages, including but not
limited to procurement of substitute goods or services; loss of use, data, or
profits, or business interruption, however caused and on any theory of
liability, whether in contract, strict liability, or tort (including
negligence), arising in any way out of the use of this software, even if
advised of the possibility of such damage.

5. CONTACT INFORMATION

Fraunhofer Institute for Integrated Circuits IIS
Attention: Division Audio and Media Technologies - MPEG-H FDK
Am Wolfsmantel 33
91058 Erlangen, Germany
www.iis.fraunhofer.de/amm
amm-info@iis.fraunhofer.de
-----------------------------------------------------------------------------*/
// System includes
#include <limits>

// External includes

// Internal includes
#include "parserutils.h"
#include "logging.h"

namespace mmt {
namespace audioparser {
namespace utils {
void skipBits(ilo::CBitParser& bitParser, uint32_t numBits) {
  if (numBits == 0) {
    return;
  }

  uint32_t pos = bitParser.tell();
  pos += numBits;
  bitParser.seek(static_cast<int32_t>(pos), ilo::EPosType::begin);
}

bool readBool(ilo::CBitParser& bitParser) {
  return (bitParser.read<uint8_t>(1) == 1);
}

uint32_t escapedValueTo32Bit(ilo::CBitParser& bitParser, uint32_t nBits1, uint32_t nBits2,
                             uint32_t nBits3) {
  uint64_t escapedValue64 = escapedValueTo64Bit(bitParser, nBits1, nBits2, nBits3);
  ILO_ASSERT(escapedValue64 <= std::numeric_limits<uint32_t>::max(),
             "The escaped value does not fit into 32 bits.");
  return static_cast<uint32_t>(escapedValue64);
}

uint64_t escapedValueTo64Bit(ilo::CBitParser& bitParser, uint32_t nBits1, uint32_t nBits2,
                             uint32_t nBits3) {
  uint64_t value = bitParser.read<uint64_t>(nBits1);
  if (value == (1u << nBits1) - 1u) {
    uint64_t valueAdd = bitParser.read<uint32_t>(nBits2);
    value += valueAdd;
    if (valueAdd == (1u << nBits2) - 1u) {
      valueAdd = bitParser.read<uint32_t>(nBits3);
      value += valueAdd;
    }
  }
  return value;
}
}  // namespace utils
}  // namespace audioparser
}  // namespace mmt
