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
#include <algorithm>

// External includes
#include "ilo/memory.h"

// Internal includes
#include "mmtaudioparser/mpeghparser.h"
#include "mpeghparserpimpl.h"
#include "logging.h"

namespace mmt {
namespace audioparser {
CMpeghParser::CMpeghParser()
    : m_mpeghPimpl(ilo::make_unique<CMpeghParser::CMpeghPimpl>()), m_validConfig(false) {}

CMpeghParser::~CMpeghParser() = default;

void CMpeghParser::addConfig(const ilo::ByteBuffer& config) {
  m_validConfig = false;
  ILO_ASSERT(!config.empty(), "The Parameter config is not allowed to be empty");
  m_mpeghPimpl->addConfig(config);
  m_validConfig = true;
}

bool CMpeghParser::isValidConfig() const {
  return m_validConfig;
}

CMpeghParser::SConfigInfo CMpeghParser::getConfigInfo() const {
  ILO_ASSERT(m_validConfig, "No vaild config read, so info about the config possible");

  SConfigInfo info{};
  info.profileLevelIndicator = m_mpeghPimpl->m_config.mpegh3daProfileLevelIndicator;
  info.samplingFrequencyIndex = m_mpeghPimpl->m_config.usacSamplingFrequencyIndex;
  info.samplingFrequency = m_mpeghPimpl->m_config.usacSamplingFrequency;
  info.coreSbrFrameLengthIndex = m_mpeghPimpl->m_config.coreSbrFrameLengthIndex;
  info.cfg_reserved = m_mpeghPimpl->m_config.cfg_reserved;
  info.receiverDelayCompensation = m_mpeghPimpl->m_config.receiverDelayCompensation;
  info.referenceLayout.speakerLayoutType = m_mpeghPimpl->m_config.referenceLayout.speakerLayoutType;
  info.referenceLayout.CICPIdx = m_mpeghPimpl->m_config.referenceLayout.CICPspeakerLayoutIdx;
  info.referenceLayout.CICPSpeakerIdx = m_mpeghPimpl->m_config.referenceLayout.CICPspeakerIdx;
  info.referenceLayout.numSpeakers = m_mpeghPimpl->m_config.referenceLayout.numSpeakers;
  info.numAudioChannels = m_mpeghPimpl->m_config.signals.numAudioChannels;
  info.numAudioObjects = m_mpeghPimpl->m_config.signals.numAudioObjects;
  info.numSAOCTransportChannels = m_mpeghPimpl->m_config.signals.numSAOCTransportChannels;
  info.numHOATransportChannels = m_mpeghPimpl->m_config.signals.numHOATransportChannels;
  info.audioPreRollPresent = m_mpeghPimpl->m_config.audioPreRollPresent;

  for (const auto& signalGroup : m_mpeghPimpl->m_config.signals.signalGroups) {
    SSignalGroup sigGrp;

    sigGrp.signalGroupType = signalGroup.signalGroupType;
    sigGrp.metaDataElementIds = signalGroup.metaDataElementIds;
    sigGrp.numSignals = signalGroup.bsNumberOfSignals + 1;

    if (signalGroup.differsFromReferenceLayout) {
      SSpeakerConfig3d audioChannelLayout;
      audioChannelLayout.numSpeakers = signalGroup.audioChannelLayout.numSpeakers;
      audioChannelLayout.CICPIdx = signalGroup.audioChannelLayout.CICPspeakerLayoutIdx;
      audioChannelLayout.CICPSpeakerIdx = signalGroup.audioChannelLayout.CICPspeakerIdx;
      audioChannelLayout.speakerLayoutType = signalGroup.audioChannelLayout.speakerLayoutType;
      sigGrp.audioChannelLayout = audioChannelLayout;
    } else {
      sigGrp.audioChannelLayout = info.referenceLayout;
    }

    info.signalGroups.push_back(sigGrp);
  }

  info.elementConfigs.reserve(m_mpeghPimpl->m_config.decoderConfig.elementConfigs.size());
  for (const auto& elementConfig : m_mpeghPimpl->m_config.decoderConfig.elementConfigs) {
    SElementConfig addElementConfig;
    addElementConfig.usacElementType = elementConfig->usacElementType;
    if (addElementConfig.usacElementType == 3) {
      const auto* extElementConfig =
          dynamic_cast<CMpeghParser::CMpeghPimpl::SExtElementConfig*>(elementConfig.get());
      ILO_ASSERT(extElementConfig != nullptr,
                 "usacElementType equals 3, but casting to SExtElementConfig returns a nullptr.");
      addElementConfig.extElementType = static_cast<uint32_t>(extElementConfig->usacExtElementType);
    } else {
      addElementConfig.extElementType = 0;
    }
    info.elementConfigs.push_back(addElementConfig);
  }
  if (m_mpeghPimpl->m_config.usacConfigExtensionPresent) {
    const auto& extensions = m_mpeghPimpl->m_config.configExtension.singleConfigExtensions;
    info.configExtensions.reserve(extensions.size());
    for (const auto& configExtension : extensions) {
      SConfigExtension addConfigExtension;
      addConfigExtension.usacConfigExtType =
          static_cast<uint32_t>(configExtension->usacConfigExtType);
      addConfigExtension.usacConfigExtLength = configExtension->usacConfigExtLength;
      info.configExtensions.push_back(addConfigExtension);

      if (configExtension->usacConfigExtType ==
          CMpeghPimpl::EUsacConfigExtType::ID_CONFIG_EXT_COMPATIBLE_PROFILELVL_SET) {
        const auto* configExt =
            dynamic_cast<CMpeghParser::CMpeghPimpl::SCompatibleProfileLevelSet*>(
                configExtension.get());
        ILO_ASSERT(configExt != nullptr,
                   "usacConfigExtType equals 7, but casting to SCompatibleProfileLevelSet returns "
                   "a nullptr.");
        info.compatibleProfileLevels = configExt->compatibleSetIndications;
      }
    }
  }
  return info;
}

static bool isLowComplexityProfile(uint8_t profileLevel) noexcept {
  // See ISO/IEC 23008-3 table 67
  return profileLevel >= 0x0B && profileLevel <= 0x0F;
}

static bool isBaselineProfile(uint8_t profileLevel) noexcept {
  // See ISO/IEC 23008-3 table 67
  return profileLevel >= 0x10 && profileLevel <= 0x14;
}

bool CMpeghParser::isLowComplexityWithBaselineCompatibleSignalling() const {
  ILO_ASSERT(m_validConfig,
             "No vaild config read, so no validation possible, if it is LC constrained Mode");

  if (!isLowComplexityProfile(m_mpeghPimpl->m_config.mpegh3daProfileLevelIndicator)) {
    return false;
  }

  if (m_mpeghPimpl->m_config.usacConfigExtensionPresent) {
    for (const auto& configExtension :
         m_mpeghPimpl->m_config.configExtension.singleConfigExtensions) {
      if (const auto* compatibleSet =
              dynamic_cast<const CMpeghPimpl::SCompatibleProfileLevelSet*>(configExtension.get())) {
        if (std::any_of(compatibleSet->compatibleSetIndications.begin(),
                        compatibleSet->compatibleSetIndications.end(), isBaselineProfile)) {
          return true;
        }
      }
    }
  }

  return false;
}
}  // namespace audioparser
}  // namespace mmt
