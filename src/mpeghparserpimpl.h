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

#pragma once

// System includes
#include <memory>
#include <vector>

// External includes
#include "ilo/bitparser.h"
#include "ilo/common_types.h"

// Internal includes
#include "mmtaudioparser/version.h"
#include "mmtaudioparser/mpeghparser.h"

namespace mmt {
namespace audioparser {
class CMpeghParser::CMpeghPimpl {
 public:
  //! As defined in ISO/IEC 23008-3
  enum class EUsacConfigExtType : uint32_t {
    ID_CONFIG_EXT_FILL = 0,
    ID_CONFIG_EXT_DOWNMIX = 1,
    ID_CONFIG_EXT_LOUDNESS_INFO = 2,
    ID_CONFIG_EXT_AUDIOSCENE_INFO = 3,
    ID_CONFIG_EXT_HOA_MATRIX = 4,
    ID_CONFIG_EXT_ICG = 5,
    ID_CONFIG_EXT_SIG_GROUP_INFO = 6,
    ID_CONFIG_EXT_COMPATIBLE_PROFILELVL_SET = 7,
  };

  struct SSingleConfigExtension {
    EUsacConfigExtType usacConfigExtType;
    uint32_t usacConfigExtLength = 0;

    virtual ~SSingleConfigExtension() noexcept = default;
  };

  struct SConfigExtension {
    std::vector<std::unique_ptr<SSingleConfigExtension>> singleConfigExtensions;
  };

  struct SCompatibleProfileLevelSet : SSingleConfigExtension {
    std::vector<uint8_t> compatibleSetIndications;
  };

  struct SElementConfig {
    uint8_t usacElementType = 0;

    virtual ~SElementConfig() noexcept = default;
  };

  struct SSbrConfig {};

  struct SMpsConfig {};

  struct S3dacoreConfig {
    bool tw_mdct = false;
    bool fullbandLpd = false;
    bool noiseFilling = false;
    bool enhancedNoiseFilling = false;
    bool igfUseEnf = false;
    bool igfUseHightRes = false;
    bool igfUseWhitening = false;
    bool igfAfterTnsSynth = false;
    uint8_t igfStartIndex = 0;
    uint8_t igfStopIndex = 0;
  };

  struct SLfeElementConfig : SElementConfig {
    S3dacoreConfig core;
  };

  struct SExtElementConfig : SElementConfig {
    uint32_t usacExtElementType = 0;
    uint32_t usacExtElementConfigLength = 0;
    bool usacExtElementDefaultLengthPresent = false;
    uint32_t usacExtElementDefaultLength = 0;
    bool usacExtElementPayloadFrag = false;
    // if also extensionElementConfigs get parsed, add a virtual struct here
  };

  struct SSingleChannelElementConfig : SElementConfig {
    S3dacoreConfig core;
    SSbrConfig sbrConfig;
  };

  struct SChannelPairElementConfig : SElementConfig {
    S3dacoreConfig core;
    bool igfIndependentTiling = false;
    SSbrConfig sbrConfig;
    uint8_t stereoConfigIdx = 0;
    SMpsConfig mpsConfig;
    uint8_t qceIndex = 0;
    bool shiftIndex0 = false;
    uint32_t shiftChannel0 = 0;
    bool shiftIndex1 = false;
    uint32_t shiftChannel1 = 0;
    bool lpdStereoIndex = 0;
  };

  struct SDecoderConfig {
    bool elementLengthPresent = false;
    std::vector<std::unique_ptr<SElementConfig>> elementConfigs;
  };

  struct SMpegh3daSpeakerDescription {
    bool isCICPspeakerIdx = false;
    uint8_t CICPspeakerIdx = 0;
    uint8_t ElevationClass = 0;
    uint8_t ElevationAngleIdx = 0;
    bool ElevationDirection = false;
    uint8_t AzimuthAngleIdx = 0;
    bool AzimuthDirection = false;
    int32_t AzimuthAngle = 0;
    int32_t ElevationAngle = 0;
    bool isLFE = false;
  };

  struct SFlexibleSpeakerConfig {
    bool angularPrecision = false;
    // NOTE: for AzimuthAngle != 0 or AzimuthAngle != 180, the following both vectors are NOT in
    // sync
    std::vector<SMpegh3daSpeakerDescription> mpegh3daSpeakerDescription;
    std::vector<bool> alsoAddSymmetricPair;
  };

  struct SSpeakerConfig3d {
    uint8_t speakerLayoutType = 0;
    uint8_t CICPspeakerLayoutIdx = 0;
    uint32_t numSpeakers = 0;
    std::vector<uint8_t> CICPspeakerIdx;
    SFlexibleSpeakerConfig flexibleSpeakerConfig;
  };

  struct SSignalGroup {
    uint8_t signalGroupType = 0;
    uint32_t bsNumberOfSignals = 0;
    bool differsFromReferenceLayout = 0;
    SSpeakerConfig3d audioChannelLayout;
    bool saocDmxLayoutPresent = false;
    SSpeakerConfig3d saocDmxChannelLayout;
    std::vector<uint8_t> metaDataElementIds;
  };

  struct SSignals3d {
    uint32_t numAudioChannels = 0;
    uint32_t numAudioObjects = 0;
    uint32_t numSAOCTransportChannels = 0;
    uint32_t numHOATransportChannels = 0;
    std::vector<SSignalGroup> signalGroups;
  };

  struct SMpegh3daConfig {
    uint8_t mpegh3daProfileLevelIndicator = 0;
    uint8_t usacSamplingFrequencyIndex = 0;
    uint32_t usacSamplingFrequency = 0;
    uint8_t coreSbrFrameLengthIndex = 0;
    bool cfg_reserved = 0;
    bool receiverDelayCompensation = 0;
    bool usacConfigExtensionPresent = 0;
    SSpeakerConfig3d referenceLayout;
    SSignals3d signals;
    SDecoderConfig decoderConfig;
    SConfigExtension configExtension;
    std::vector<uint8_t> compatibleProfileLevels;
    bool audioPreRollPresent = false;
  };

  void addConfig(const ilo::ByteBuffer& config);

  SMpegh3daConfig mpegh3daConfig(ilo::CBitParser& bitParser);
  SSignals3d signals3d(ilo::CBitParser& bitParser);
  SSpeakerConfig3d speakerConfig3d(ilo::CBitParser& bitParser);
  SFlexibleSpeakerConfig mpegh3daFlexibleSpeakerConfig(ilo::CBitParser& bitParser,
                                                       uint32_t numSpeakers);
  SMpegh3daSpeakerDescription mpegh3daSpeakerDescription(ilo::CBitParser& bitParser,
                                                         bool angularPrecision);
  SDecoderConfig mpegh3daDecoderConfig(ilo::CBitParser& bitParser, uint8_t sbrRatioIndex,
                                       uint32_t numChannels, SMpegh3daConfig& mpegh3daConfig);
  SSingleChannelElementConfig mpegh3daSingleChannelElementConfig(ilo::CBitParser& bitParser,
                                                                 uint8_t sbrRatioIndex);
  SChannelPairElementConfig mpegh3daChannelPairElementConfig(ilo::CBitParser& bitParser,
                                                             uint8_t sbrRatioIndex,
                                                             uint32_t numChannels);
  SLfeElementConfig mpegh3daLfeElementConfig();
  SExtElementConfig mpegh3daExtElementConfig(ilo::CBitParser& bitParser,
                                             SMpegh3daConfig& mpegh3daConfig);
  S3dacoreConfig mpegh3daCoreConfig(ilo::CBitParser& bitParser);
  std::unique_ptr<SCompatibleProfileLevelSet> mpegh3daCompatibleProfileLevelSet(
      ilo::CBitParser& bitParser, uint32_t configExtLength);
  SConfigExtension mpegh3daConfigExtension(ilo::CBitParser& bitParser);
  SSbrConfig sbrConfig(ilo::CBitParser& bitParser);
  SMpsConfig mps121Config(ilo::CBitParser& bitParser, uint8_t stereoConfigIdx);

  SMpegh3daConfig m_config;
};
}  // namespace audioparser
}  // namespace mmt
