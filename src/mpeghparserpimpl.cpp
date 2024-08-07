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
#include <array>
#include <cmath>
#include <map>

// External includes
#include "ilo/bitparser.h"
#include "ilo/memory.h"

// Internal includes
#include "common.h"
#include "parserutils.h"
#include "mpeghparserpimpl.h"
#include "logging.h"

namespace mmt {
namespace audioparser {
using namespace utils;

/* Table is defined in ISO/IEC 23003-3:2012, Table 67 */
static constexpr std::array<uint32_t, 0x21> samplingFrequencyIndex = {
    96000, 88200, 64000, 48000, /* 0x00 - 0x03 */
    44100, 32000, 24000, 22050, /* 0x04 - 0x07 */
    16000, 12000, 11025, 8000,  /* 0x08 - 0x0b */
    7350,  0,     0,     57600, /* 0x0c - 0x0f */
    51200, 40000, 38400, 34150, /* 0x10 - 0x13 */
    28800, 25600, 20000, 19200, /* 0x14 - 0x17 */
    17075, 14400, 12800, 9600,  /* 0x18 - 0x1b */
    0,     0,     0,     0      /* 0x1c - 0x1f, 0x20 */
};

/* Table is defined in ISO/IEC 23003-3:2012, Table 67 */
static constexpr std::array<int32_t, 0x80> CICPLoudspeakerIndexAzimuth = {
    30,    -30,   0,     0,     110,   -110,  22,    -22,   /* 0x00 - 0x07 */
    135,   -135,  180,   -9999, -9999, 90,    -90,   60,    /* 0x08 - 0x0f */
    -60,   30,    -30,   0,     135,   -135,  180,   90,    /* 0x10 - 0x17 */
    -90,   0,     45,    45,    -45,   0,     110,   -110,  /* 0x18 - 0x1f */
    45,    -45,   45,    -45,   -45,   -1111, -1111, -1111, /* 0x20 - 0x27 */
    -1111, 150,   -150,  -9999, -9999, -9999, -9999, -9999, /* 0x27 - 0x2f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x30 - 0x37 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x38 - 0x3f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x40 - 0x47 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x48 - 0x4f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x50 - 0x57 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x58 - 0x5f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x60 - 0x67 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x68 - 0x6f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x70 - 0x77 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x78 - 0x7f */
};

static constexpr std::array<int32_t, 0x80> CICPLoudspeakerIndexElevation = {
    0,     0,     0,     -15,   0,     0,     0,     0,     /* 0x00 - 0x07 */
    0,     0,     0,     -9999, -9999, 0,     0,     0,     /* 0x08 - 0x0f */
    0,     35,    35,    35,    35,    35,    35,    35,    /* 0x10 - 0x17 */
    35,    90,    -15,   -15,   -15,   -15,   35,    35,    /* 0x18 - 0x1f */
    35,    35,    0,     0,     -15,   0,     0,     0,     /* 0x20 - 0x27 */
    0,     0,     0,     -9999, -9999, -9999, -9999, -9999, /* 0x27 - 0x2f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x30 - 0x37 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x38 - 0x3f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x40 - 0x47 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x48 - 0x4f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x50 - 0x57 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x58 - 0x5f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x60 - 0x67 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x68 - 0x6f */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x70 - 0x77 */
    -9999, -9999, -9999, -9999, -9999, -9999, -9999, -9999, /* 0x78 - 0x7f */
};

static constexpr std::array<bool, 0x80> CICPLoudspeakerIndexIsLFE = {
    false, false, false, true,  false, false, false, false, /* 0x00 - 0x07 */
    false, false, false, false, false, false, false, false, /* 0x08 - 0x0f */
    false, false, true,  false, false, false, false, false, /* 0x10 - 0x17 */
    false, false, false, false, false, false, false, false, /* 0x18 - 0x1f */
    false, false, false, false, true,  false, false, false, /* 0x20 - 0x27 */
    false, false, false, false, false, false, false, false, /* 0x27 - 0x2f */
    false, false, false, false, false, false, false, false, /* 0x30 - 0x37 */
    false, false, false, false, false, false, false, false, /* 0x38 - 0x3f */
    false, false, false, false, false, false, false, false, /* 0x40 - 0x47 */
    false, false, false, false, false, false, false, false, /* 0x48 - 0x4f */
    false, false, false, false, false, false, false, false, /* 0x50 - 0x57 */
    false, false, false, false, false, false, false, false, /* 0x58 - 0x5f */
    false, false, false, false, false, false, false, false, /* 0x60 - 0x67 */
    false, false, false, false, false, false, false, false, /* 0x68 - 0x6f */
    false, false, false, false, false, false, false, false, /* 0x70 - 0x77 */
    false, false, false, false, false, false, false, false, /* 0x78 - 0x7f */
};

void CMpeghParser::CMpeghPimpl::addConfig(const ilo::ByteBuffer& config) {
  ilo::CBitParser bitParser(config);
  m_config = mpegh3daConfig(bitParser);
  uint32_t bitsLeft = bitParser.nofBitsLeft();
  ILO_ASSERT(bitsLeft < 8,
             "%i number of bits left after reading the config. There are not more than 7 allowed",
             bitsLeft);
}

CMpeghParser::CMpeghPimpl::SSbrConfig CMpeghParser::CMpeghPimpl::sbrConfig(
    ilo::CBitParser& /*bitParser*/) {
  ILO_ASSERT(false, "SBR-config not implemented until now");
  return SSbrConfig{};
}

CMpeghParser::CMpeghPimpl::SMpsConfig CMpeghParser::CMpeghPimpl::mps121Config(
    ilo::CBitParser& /*bitParser*/, uint8_t /*stereoConfigIdx*/) {
  ILO_ASSERT(false, "MPS212-config not implemented until now");
  return SMpsConfig{};
}

CMpeghParser::CMpeghPimpl::SMpegh3daConfig CMpeghParser::CMpeghPimpl::mpegh3daConfig(
    ilo::CBitParser& bitParser) {
  SMpegh3daConfig mpegh3daConfig;

  mpegh3daConfig.mpegh3daProfileLevelIndicator = bitParser.read<uint8_t>(8);
  mpegh3daConfig.usacSamplingFrequencyIndex = bitParser.read<uint8_t>(5);
  if (mpegh3daConfig.usacSamplingFrequencyIndex == 0x1f) {
    mpegh3daConfig.usacSamplingFrequency = bitParser.read<uint32_t>(24);
  } else {
    mpegh3daConfig.usacSamplingFrequency =
        samplingFrequencyIndex.at(mpegh3daConfig.usacSamplingFrequencyIndex);
  }

  mpegh3daConfig.coreSbrFrameLengthIndex = bitParser.read<uint8_t>(3);
  mpegh3daConfig.cfg_reserved = readBool(bitParser);
  mpegh3daConfig.receiverDelayCompensation = readBool(bitParser);

  mpegh3daConfig.referenceLayout = speakerConfig3d(bitParser);
  mpegh3daConfig.signals = signals3d(bitParser);
  uint32_t numberChannels = mpegh3daConfig.signals.numAudioChannels +
                            mpegh3daConfig.signals.numAudioObjects +
                            mpegh3daConfig.signals.numHOATransportChannels +
                            mpegh3daConfig.signals.numSAOCTransportChannels;
  uint8_t sbrRatioIndex = 0;
  ILO_ASSERT(mpegh3daConfig.coreSbrFrameLengthIndex <= 4, "SBRCoreFrameLengthIndex is invalid");
  switch (mpegh3daConfig.coreSbrFrameLengthIndex) {
    case 0:
    case 1:
      sbrRatioIndex = 0;
      break;
    case 2:
      sbrRatioIndex = 2;
      break;
    case 3:
      sbrRatioIndex = 3;
      break;
    case 4:
      sbrRatioIndex = 1;
      break;
    default:
      ILO_ASSERT(false, "Invalid value for coreSbrFrameLengthIndex found.");
  }
  mpegh3daConfig.decoderConfig =
      mpegh3daDecoderConfig(bitParser, sbrRatioIndex, numberChannels, mpegh3daConfig);

  mpegh3daConfig.usacConfigExtensionPresent = readBool(bitParser);
  if (mpegh3daConfig.usacConfigExtensionPresent) {
    mpegh3daConfig.configExtension = mpegh3daConfigExtension(bitParser);
  }

  return mpegh3daConfig;
}

CMpeghParser::CMpeghPimpl::SSignals3d CMpeghParser::CMpeghPimpl::signals3d(
    ilo::CBitParser& bitParser) {
  SSignals3d signals;
  uint8_t currentMetaDataElementId = 0;
  signals.signalGroups.resize(bitParser.read<uint8_t>(5) + 1u);
  for (auto& signalGroup : signals.signalGroups) {
    signalGroup.signalGroupType = bitParser.read<uint8_t>(3);
    signalGroup.bsNumberOfSignals = escapedValueTo32Bit(bitParser, 5, 8, 16);
    // SignalGroupTypeChannels
    if (signalGroup.signalGroupType == 0x0) {
      signals.numAudioChannels += signalGroup.bsNumberOfSignals + 1;
      signalGroup.differsFromReferenceLayout = readBool(bitParser);
      if (signalGroup.differsFromReferenceLayout) {
        signalGroup.audioChannelLayout = speakerConfig3d(bitParser);
      }

      for (uint32_t offset = 0; offset < signalGroup.bsNumberOfSignals + 1; offset++) {
        signalGroup.metaDataElementIds.push_back(currentMetaDataElementId);
        currentMetaDataElementId++;
      }
    }
    // SignalGroupTypeObject
    if (signalGroup.signalGroupType == 0x1) {
      signals.numAudioObjects += signalGroup.bsNumberOfSignals + 1;

      for (uint32_t offset = 0; offset < signalGroup.bsNumberOfSignals + 1; offset++) {
        signalGroup.metaDataElementIds.push_back(currentMetaDataElementId);
        currentMetaDataElementId++;
      }
    }
    // SignalGroupTypeSAOC
    if (signalGroup.signalGroupType == 0x2) {
      signals.numSAOCTransportChannels += signalGroup.bsNumberOfSignals + 1;
      signalGroup.saocDmxLayoutPresent = readBool(bitParser);
      if (signalGroup.saocDmxLayoutPresent) {
        signalGroup.saocDmxChannelLayout = speakerConfig3d(bitParser);
      }
    }
    // SignalGroupTypeHOA
    if (signalGroup.signalGroupType == 0x3) {
      signals.numHOATransportChannels += signalGroup.bsNumberOfSignals + 1;

      signalGroup.metaDataElementIds.push_back(currentMetaDataElementId);
      currentMetaDataElementId++;
    }

    ILO_ASSERT(signalGroup.signalGroupType < 0x4, "Config is invalid. Not defined signalGroupType");
  }
  return signals;
}

CMpeghParser::CMpeghPimpl::SSpeakerConfig3d CMpeghParser::CMpeghPimpl::speakerConfig3d(
    ilo::CBitParser& bitParser) {
  SSpeakerConfig3d speakerConfig;

  speakerConfig.speakerLayoutType = bitParser.read<uint8_t>(2);
  if (speakerConfig.speakerLayoutType == 0) {
    speakerConfig.CICPspeakerLayoutIdx = bitParser.read<uint8_t>(6);

    // CICPspeakerLayoutIdx
    const std::map<uint8_t, uint32_t> NUM_SPEAKERS{
        {uint8_t(1), 1},   {uint8_t(2), 2},   {uint8_t(3), 3},   {uint8_t(4), 4},
        {uint8_t(5), 5},   {uint8_t(6), 6},   {uint8_t(7), 8},   {uint8_t(8), 2},
        {uint8_t(9), 3},   {uint8_t(10), 4},  {uint8_t(11), 6},  {uint8_t(12), 8},
        {uint8_t(13), 24}, {uint8_t(14), 8},  {uint8_t(15), 12}, {uint8_t(16), 10},
        {uint8_t(17), 12}, {uint8_t(18), 14}, {uint8_t(19), 12}, {uint8_t(20), 14}};

    ILO_ASSERT(NUM_SPEAKERS.count(speakerConfig.CICPspeakerLayoutIdx) != 0,
               "No valid cicp index found.");
    speakerConfig.numSpeakers = NUM_SPEAKERS.at(speakerConfig.CICPspeakerLayoutIdx);
  } else {
    speakerConfig.numSpeakers = escapedValueTo32Bit(bitParser, 5, 8, 16) + 1;
    if (speakerConfig.speakerLayoutType == 1) {
      speakerConfig.CICPspeakerIdx.clear();
      for (uint32_t i = 0; i < speakerConfig.numSpeakers; i++) {
        speakerConfig.CICPspeakerIdx.push_back(bitParser.read<uint8_t>(7));
      }
    }
    if (speakerConfig.speakerLayoutType == 2) {
      speakerConfig.flexibleSpeakerConfig =
          mpegh3daFlexibleSpeakerConfig(bitParser, speakerConfig.numSpeakers);
    }
  }
  return speakerConfig;
}

CMpeghParser::CMpeghPimpl::SFlexibleSpeakerConfig
CMpeghParser::CMpeghPimpl::mpegh3daFlexibleSpeakerConfig(ilo::CBitParser& bitParser,
                                                         uint32_t numSpeakers) {
  SFlexibleSpeakerConfig flexibleSpeakerConfig;
  flexibleSpeakerConfig.angularPrecision = readBool(bitParser);
  flexibleSpeakerConfig.mpegh3daSpeakerDescription.clear();
  flexibleSpeakerConfig.alsoAddSymmetricPair.clear();
  for (uint32_t i = 0; i < numSpeakers; i++) {
    SMpegh3daSpeakerDescription newSpeakerDescription =
        mpegh3daSpeakerDescription(bitParser, flexibleSpeakerConfig.angularPrecision);
    flexibleSpeakerConfig.mpegh3daSpeakerDescription.push_back(newSpeakerDescription);
    if (newSpeakerDescription.AzimuthAngle != 0 && newSpeakerDescription.AzimuthAngle != 180) {
      bool tmp = readBool(bitParser);
      flexibleSpeakerConfig.alsoAddSymmetricPair.push_back(tmp);
      if (tmp) {
        i++;
      }
    }
  }
  return flexibleSpeakerConfig;
}

CMpeghParser::CMpeghPimpl::SMpegh3daSpeakerDescription
CMpeghParser::CMpeghPimpl::mpegh3daSpeakerDescription(ilo::CBitParser& bitParser,
                                                      bool angularPrecision) {
  SMpegh3daSpeakerDescription mpegh3daSpeakerDescription;
  mpegh3daSpeakerDescription.isCICPspeakerIdx = readBool(bitParser);
  if (mpegh3daSpeakerDescription.isCICPspeakerIdx) {
    mpegh3daSpeakerDescription.CICPspeakerIdx = bitParser.read<uint8_t>(7);
    mpegh3daSpeakerDescription.AzimuthAngle =
        CICPLoudspeakerIndexAzimuth.at(mpegh3daSpeakerDescription.CICPspeakerIdx);
    mpegh3daSpeakerDescription.ElevationAngle =
        CICPLoudspeakerIndexElevation.at(mpegh3daSpeakerDescription.CICPspeakerIdx);
    mpegh3daSpeakerDescription.isLFE =
        CICPLoudspeakerIndexIsLFE.at(mpegh3daSpeakerDescription.CICPspeakerIdx);
  } else {
    mpegh3daSpeakerDescription.ElevationClass = bitParser.read<uint8_t>(2);
    if (mpegh3daSpeakerDescription.ElevationClass == 3) {
      if (angularPrecision) {
        mpegh3daSpeakerDescription.ElevationAngleIdx = bitParser.read<uint8_t>(7);
      } else {
        mpegh3daSpeakerDescription.ElevationAngleIdx = bitParser.read<uint8_t>(5);
      }
      if (mpegh3daSpeakerDescription.ElevationAngleIdx != 0) {
        mpegh3daSpeakerDescription.ElevationDirection = readBool(bitParser);
      }
    } else {
      mpegh3daSpeakerDescription.ElevationAngleIdx = 0;
      mpegh3daSpeakerDescription.ElevationDirection = false;
    }
    if (angularPrecision) {
      mpegh3daSpeakerDescription.AzimuthAngleIdx = bitParser.read<uint8_t>(8);
    } else {
      mpegh3daSpeakerDescription.AzimuthAngleIdx = bitParser.read<uint8_t>(6);
    }

    if (angularPrecision) {
      mpegh3daSpeakerDescription.AzimuthAngle = mpegh3daSpeakerDescription.AzimuthAngleIdx;
      mpegh3daSpeakerDescription.ElevationAngle = mpegh3daSpeakerDescription.ElevationAngleIdx;
    } else {
      mpegh3daSpeakerDescription.AzimuthAngle = mpegh3daSpeakerDescription.AzimuthAngleIdx * 5;
      mpegh3daSpeakerDescription.ElevationAngle = mpegh3daSpeakerDescription.ElevationAngleIdx * 5;
    }
    if (mpegh3daSpeakerDescription.ElevationDirection) {
      mpegh3daSpeakerDescription.ElevationAngle *= -1;
    }
    if (mpegh3daSpeakerDescription.AzimuthAngle != 0 &&
        (mpegh3daSpeakerDescription.AzimuthAngle != 180)) {
      mpegh3daSpeakerDescription.AzimuthDirection = readBool(bitParser);
      if (mpegh3daSpeakerDescription.AzimuthDirection) {
        mpegh3daSpeakerDescription.AzimuthAngle *= -1;
      }
    }
    mpegh3daSpeakerDescription.isLFE = readBool(bitParser);
  }
  return mpegh3daSpeakerDescription;
}

CMpeghParser::CMpeghPimpl::SDecoderConfig CMpeghParser::CMpeghPimpl::mpegh3daDecoderConfig(
    ilo::CBitParser& bitParser, uint8_t sbrRatioIndex, uint32_t numChannels,
    SMpegh3daConfig& mpegh3daConfig) {
  SDecoderConfig decoderConfig;
  auto numElements = escapedValueTo32Bit(bitParser, 4, 8, 16) + 1;
  decoderConfig.elementLengthPresent = readBool(bitParser);
  decoderConfig.elementConfigs.reserve(numElements);
  for (uint32_t elemIdx = 0; elemIdx < numElements; elemIdx++) {
    switch (static_cast<EUsacElementType>(bitParser.read<uint8_t>(2))) {
      case EUsacElementType::ID_USAC_SCE: {
        SSingleChannelElementConfig singleElement =
            mpegh3daSingleChannelElementConfig(bitParser, sbrRatioIndex);
        decoderConfig.elementConfigs.push_back(
            ilo::make_unique<SSingleChannelElementConfig>(singleElement));
        break;
      }
      case EUsacElementType::ID_USAC_CPE: {
        SChannelPairElementConfig channelPairElement =
            mpegh3daChannelPairElementConfig(bitParser, sbrRatioIndex, numChannels);
        decoderConfig.elementConfigs.push_back(
            ilo::make_unique<SChannelPairElementConfig>(channelPairElement));
        break;
      }
      case EUsacElementType::ID_USAC_LFE: {
        SLfeElementConfig lfeElement = mpegh3daLfeElementConfig();
        decoderConfig.elementConfigs.push_back(ilo::make_unique<SLfeElementConfig>(lfeElement));
        break;
      }
      case EUsacElementType::ID_USAC_EXT: {
        SExtElementConfig extElement = mpegh3daExtElementConfig(bitParser, mpegh3daConfig);
        decoderConfig.elementConfigs.push_back(ilo::make_unique<SExtElementConfig>(extElement));
        break;
      }
      default:
        ILO_ASSERT(false, "Invalid value for extension element type found.");
    }
  }
  return decoderConfig;
}

CMpeghParser::CMpeghPimpl::SSingleChannelElementConfig
CMpeghParser::CMpeghPimpl::mpegh3daSingleChannelElementConfig(ilo::CBitParser& bitParser,
                                                              uint8_t sbrRatioIndex) {
  SSingleChannelElementConfig singleChannelElementConfig;
  singleChannelElementConfig.usacElementType = 0;
  singleChannelElementConfig.core = mpegh3daCoreConfig(bitParser);
  if (sbrRatioIndex > 0) {
    singleChannelElementConfig.sbrConfig = sbrConfig(bitParser);
  } else {
    singleChannelElementConfig.sbrConfig = SSbrConfig{};
  }
  return singleChannelElementConfig;
}

CMpeghParser::CMpeghPimpl::SChannelPairElementConfig
CMpeghParser::CMpeghPimpl::mpegh3daChannelPairElementConfig(ilo::CBitParser& bitParser,
                                                            uint8_t sbrRatioIndex,
                                                            uint32_t numChannels) {
  SChannelPairElementConfig channelPairElementConfig;
  channelPairElementConfig.usacElementType = 1;
  ILO_ASSERT(numChannels > 1, "numberOfChannels must be at least 2");
  channelPairElementConfig.core = mpegh3daCoreConfig(bitParser);
  if (channelPairElementConfig.core.enhancedNoiseFilling) {
    channelPairElementConfig.igfIndependentTiling = readBool(bitParser);
  } else {
    channelPairElementConfig.igfIndependentTiling = false;
  }
  if (sbrRatioIndex > 0) {
    channelPairElementConfig.sbrConfig = sbrConfig(bitParser);
    channelPairElementConfig.stereoConfigIdx = bitParser.read<uint8_t>(2);
  } else {
    channelPairElementConfig.stereoConfigIdx = 0;
    channelPairElementConfig.sbrConfig = SSbrConfig{};
  }

  if (channelPairElementConfig.stereoConfigIdx > 0) {
    channelPairElementConfig.mpsConfig =
        mps121Config(bitParser, channelPairElementConfig.stereoConfigIdx);
  } else {
    channelPairElementConfig.mpsConfig = SMpsConfig{};
  }

  uint32_t nBits = static_cast<uint32_t>(std::floor(std::log2(numChannels - 1))) + 1;
  channelPairElementConfig.qceIndex = bitParser.read<uint8_t>(2);
  if (channelPairElementConfig.qceIndex > 0) {
    channelPairElementConfig.shiftIndex0 = readBool(bitParser);
    if (channelPairElementConfig.shiftIndex0) {
      channelPairElementConfig.shiftChannel0 = bitParser.read<uint32_t>(nBits);
    } else {
      channelPairElementConfig.shiftChannel0 = 0;
    }
  } else {
    channelPairElementConfig.shiftIndex0 = false;
    channelPairElementConfig.shiftChannel0 = 0;
  }

  channelPairElementConfig.shiftIndex1 = readBool(bitParser);
  if (channelPairElementConfig.shiftIndex1) {
    channelPairElementConfig.shiftChannel1 = bitParser.read<uint32_t>(nBits);
  } else {
    channelPairElementConfig.shiftChannel1 = 0;
  }

  if (sbrRatioIndex == 0 && channelPairElementConfig.qceIndex == 0) {
    channelPairElementConfig.lpdStereoIndex = readBool(bitParser);
  } else {
    channelPairElementConfig.lpdStereoIndex = false;
  }

  return channelPairElementConfig;
}

CMpeghParser::CMpeghPimpl::SLfeElementConfig CMpeghParser::CMpeghPimpl::mpegh3daLfeElementConfig() {
  SLfeElementConfig lfeElement;

  lfeElement.usacElementType = 2;
  lfeElement.core.tw_mdct = false;
  lfeElement.core.fullbandLpd = false;
  lfeElement.core.noiseFilling = false;
  lfeElement.core.enhancedNoiseFilling = false;

  lfeElement.core.igfUseEnf = false;
  lfeElement.core.igfUseHightRes = false;
  lfeElement.core.igfUseWhitening = false;
  lfeElement.core.igfAfterTnsSynth = false;
  lfeElement.core.igfStartIndex = 0;
  lfeElement.core.igfStopIndex = 0;

  return lfeElement;
}

CMpeghParser::CMpeghPimpl::SExtElementConfig CMpeghParser::CMpeghPimpl::mpegh3daExtElementConfig(
    ilo::CBitParser& bitParser, SMpegh3daConfig& mpegh3daConfig) {
  SExtElementConfig extElement{};

  extElement.usacElementType = 3;
  extElement.usacExtElementType = escapedValueTo32Bit(bitParser, 4, 8, 16);
  extElement.usacExtElementConfigLength = escapedValueTo32Bit(bitParser, 4, 8, 16);
  extElement.usacExtElementDefaultLengthPresent = readBool(bitParser);
  if (extElement.usacExtElementDefaultLengthPresent) {
    extElement.usacExtElementDefaultLength = escapedValueTo32Bit(bitParser, 8, 16, 0) + 1;
  } else {
    extElement.usacExtElementDefaultLength = 0;
  }
  extElement.usacExtElementPayloadFrag = readBool(bitParser);
  switch (extElement.usacExtElementType) {
      // ID_EXT_ELE_FILL
    case 0:
      ILO_ASSERT(extElement.usacExtElementConfigLength == 0,
                 "ID_EXT_ELE_FILL is not allowed to have a Config Length");
      break;
      // ID_EXT_ELE_AUDIOPREROLL
    case 3:
      mpegh3daConfig.audioPreRollPresent = true;
      ILO_ASSERT(extElement.usacExtElementConfigLength == 0,
                 "ID_EXT_ELE_AUDIOPREROLL is not allowed to have a Config Length");
      break;
    default:
      skipBits(bitParser, extElement.usacExtElementConfigLength * 8);
      break;
  }

  return extElement;
}

CMpeghParser::CMpeghPimpl::S3dacoreConfig CMpeghParser::CMpeghPimpl::mpegh3daCoreConfig(
    ilo::CBitParser& bitParser) {
  S3dacoreConfig coreConfig;

  coreConfig.tw_mdct = readBool(bitParser);
  coreConfig.fullbandLpd = readBool(bitParser);
  coreConfig.noiseFilling = readBool(bitParser);
  coreConfig.enhancedNoiseFilling = readBool(bitParser);
  if (coreConfig.enhancedNoiseFilling) {
    coreConfig.igfUseEnf = readBool(bitParser);
    coreConfig.igfUseHightRes = readBool(bitParser);
    coreConfig.igfUseWhitening = readBool(bitParser);
    coreConfig.igfAfterTnsSynth = readBool(bitParser);
    coreConfig.igfStartIndex = bitParser.read<uint8_t>(5);
    coreConfig.igfStopIndex = bitParser.read<uint8_t>(4);
  } else {
    coreConfig.igfUseEnf = false;
    coreConfig.igfUseHightRes = false;
    coreConfig.igfUseWhitening = false;
    coreConfig.igfAfterTnsSynth = false;
    coreConfig.igfStartIndex = 0;
    coreConfig.igfStopIndex = 0;
  }

  return coreConfig;
}

std::unique_ptr<CMpeghParser::CMpeghPimpl::SCompatibleProfileLevelSet>
CMpeghParser::CMpeghPimpl::mpegh3daCompatibleProfileLevelSet(ilo::CBitParser& bitParser,
                                                             uint32_t configExtLength) {
  SCompatibleProfileLevelSet compProfLvlSet;

  compProfLvlSet.usacConfigExtType = EUsacConfigExtType::ID_CONFIG_EXT_COMPATIBLE_PROFILELVL_SET;
  compProfLvlSet.usacConfigExtLength = configExtLength;

  auto numCompatibleSets = static_cast<uint8_t>(bitParser.read<uint8_t>(4) + 1U);

  // reserved
  bitParser.read<uint8_t>(4);

  for (uint8_t i = 0; i < numCompatibleSets; i++) {
    compProfLvlSet.compatibleSetIndications.push_back(bitParser.read<uint8_t>(8));
  }

  return ilo::make_unique<SCompatibleProfileLevelSet>(compProfLvlSet);
}

CMpeghParser::CMpeghPimpl::SConfigExtension CMpeghParser::CMpeghPimpl::mpegh3daConfigExtension(
    ilo::CBitParser& bitParser) {
  SConfigExtension configExtension;
  auto numConfigExtensions = escapedValueTo32Bit(bitParser, 2, 4, 8) + 1;
  configExtension.singleConfigExtensions.reserve(numConfigExtensions);
  for (uint32_t i = 0; i < numConfigExtensions; i++) {
    auto configExtType = static_cast<EUsacConfigExtType>(escapedValueTo32Bit(bitParser, 4, 8, 16));
    uint32_t configExtLength = escapedValueTo32Bit(bitParser, 4, 8, 16);

    SSingleConfigExtension singleConfigExtension;
    singleConfigExtension.usacConfigExtType = configExtType;
    singleConfigExtension.usacConfigExtLength = configExtLength;

    switch (configExtType) {
      case EUsacConfigExtType::ID_CONFIG_EXT_FILL: {
        for (uint32_t j = 0; j < singleConfigExtension.usacConfigExtLength; j++) {
          uint8_t val = bitParser.read<uint8_t>(8);
          if (val != 0xA5) {
            ILO_LOG_WARNING(
                "Fill ExElement has wrong digits, the value should be 0xA5, but it is %02x", val);
          }
        }
        configExtension.singleConfigExtensions.push_back(
            ilo::make_unique<SSingleConfigExtension>(singleConfigExtension));
        break;
      }
      case EUsacConfigExtType::ID_CONFIG_EXT_COMPATIBLE_PROFILELVL_SET: {
        configExtension.singleConfigExtensions.push_back(
            mpegh3daCompatibleProfileLevelSet(bitParser, configExtLength));
        break;
      }
      default:
        skipBits(bitParser, singleConfigExtension.usacConfigExtLength * 8);
        configExtension.singleConfigExtensions.push_back(
            ilo::make_unique<SSingleConfigExtension>(singleConfigExtension));
        break;
    }
  }

  return configExtension;
}
}  // namespace audioparser
}  // namespace mmt
