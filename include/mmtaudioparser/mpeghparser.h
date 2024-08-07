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

/*!
 * @file mpeghparser.h
 *
 * @brief Audio config parser implementation for MPEG-H 3D Audio.
 */

#pragma once

// System includes
#include <cstdint>
#include <memory>
#include <vector>

// External includes
#include "ilo/common_types.h"

// Internal includes
#include "mmtaudioparser/version.h"
#include "mmtaudioparser/mmtaudioparser.h"

namespace mmt {
namespace audioparser {
/*!
 * @brief Parser for MPEG-H 3D Audio configuration structure.
 *
 * The 3D Audio coding is defined the MPEG-H standard (ISO/IEC 23008-3).
 */
class CMpeghParser : public IAudioParser {
 public:
  /*!
   * Base information for USAC configuration extensions contained in the mpegh3daConfigExtension()
   * structure.
   */
  struct SConfigExtension {
    //! The type indicator of the USAC configuration extension.
    uint32_t usacConfigExtType = 0;
    //! The number of bytes the USAC configuration extension uses.
    uint32_t usacConfigExtLength = 0;
  };

  //! Base information for element configurations in the mpegh3daDecoderConfig() structure.
  struct SElementConfig {
    //! The type indicator for the element configuration.
    uint32_t usacElementType = 0;
    //! The extension element type indicator for the element configuration.
    uint32_t extElementType = 0;
  };

  //! Representation of the speakerConfig3d() structure.
  struct SSpeakerConfig3d {
    /*!
     * @brief The type of speaker layout represented by this structure.
     *
     * Values of 0 and 1 indicate the loudspeaker layout is signalled by the ChannelConfiguration
     * and LoudspeakerGeometry indices respectively, as defined in ISO/IEC 23091-3.
     * A value of 2 indicates that loudspeaker layout is signalled by means of a list of explicit
     * geometric position information.
     * A value of 3 indicates the absence of any associated rendering layout (Contribution Mode).
     */
    uint8_t speakerLayoutType = 0;
    //! The ChannelConfiguration value as defined in ISO/IEC 23091-3 for speakerLayoutType of 0.
    uint8_t CICPIdx = 0;
    //! The number of loudspeakers of this speaker configuration.
    uint32_t numSpeakers = 0;
    //! The LoudspeakerGeometry values as defined in ISO/IEC 23091-3 for non-zero speakerLayoutType.
    std::vector<uint8_t> CICPSpeakerIdx;
  };

  //! Representation of a signal group as defined in the signals3d() structure.
  struct SSignalGroup {
    //! The type indicator of the signal group.
    uint8_t signalGroupType = 255;
    //! The associated element IDs depending on the signal group type.
    std::vector<uint8_t> metaDataElementIds;
    /*!
     * The effective SpeakerConfig3d() for this signal group, either defined in-line or the
     * reference layout.
     */
    SSpeakerConfig3d audioChannelLayout;
    //! The number of signals in this signal group.
    uint32_t numSignals = 0;
  };

  //! Representation of the mpegh3daConfig() and its children structure.
  struct SConfigInfo {
    //! Indication of the MPEG-H 3D audio profile and level according to ISO/IEC 23008-3 table 67.
    uint8_t profileLevelIndicator = 0;
    /*!
     * The index into the USAC sampling frequency mapping, as defined in ISO/IEC 23003-3
     * subclause 6.
     */
    uint8_t samplingFrequencyIndex = 0;
    //! The effective sampling frequency in Hz.
    uint32_t samplingFrequency = 0;
    /*!
     * The index into the SBR and output frame length mapping, as defined in ISO/IEC 23003-3
     * subclause 6.
     */
    uint8_t coreSbrFrameLengthIndex = 0;
    //! Reserved value, ignore.
    bool cfg_reserved = false;
    //! Force decoder to operate in constant delay.
    bool receiverDelayCompensation = false;
    //! Reference speakerConfig3d() structure the audio content is produced for.
    SSpeakerConfig3d referenceLayout;
    //! The total number of audio channels in all signal groups.
    uint32_t numAudioChannels = 0;
    //! The total number of audio objects in all signal groups.
    uint32_t numAudioObjects = 0;
    /*!
     * The total number of Spatial Audio Object Coding (SAOC) transport channels in all signal
     * groups.
     */
    uint32_t numSAOCTransportChannels = 0;
    //! The total number of higher order ambisonics (HOA) transport channels in all signal groups.
    uint32_t numHOATransportChannels = 0;
    //! The signal groups contained in this configuration.
    std::vector<SSignalGroup> signalGroups;
    //! The element configuration entries in this configuration.
    std::vector<SElementConfig> elementConfigs;
    //! The USAC configuration extensions in this configuration.
    std::vector<SConfigExtension> configExtensions;
    /*!
     * The compatible profile level sets as defined by the CompatibleProfileLevelSet() config
     * extension.
     */
    std::vector<uint8_t> compatibleProfileLevels;
    /*!
     * Whether audio pre-roll is present as defined by the ID_EXT_ELE_AUDIOPREROLL USAC extension
     * element type.
     */
    bool audioPreRollPresent = false;
  };

  CMpeghParser();
  ~CMpeghParser() override;

  /*!
   * @brief Feeds in a new binary config buffer.
   *
   * This function parses the given config buffer and fills in the MPEG-H 3D Audio configuration
   * structure, overwriting any previously extracted configuration.
   *
   * @param [in] config - the binary MPEG-H 3D Audio configuration structure
   */
  void addConfig(const ilo::ByteBuffer& config) override;

  /*!
   * @brief Returns whether the last read binary configuration structure contains a valid MPEG-H 3D
   * Audio configuration structure.
   *
   * On an empty parser (no binary configuration buffer has been read so far), false is returned.
   */
  bool isValidConfig() const override;

  /*!
   * @returns the last read MPEG-H 3D Audio configuration info structure.
   */
  SConfigInfo getConfigInfo() const;

  /*!
   * @brief Returns whether the bitstream profile is low complexity and signals baseline
   * compatibility.
   *
   * @note If the bitstream profile itself is baseline, this function returns false.
   */
  bool isLowComplexityWithBaselineCompatibleSignalling() const;

  class CMpeghPimpl;

 private:
  std::unique_ptr<CMpeghPimpl> m_mpeghPimpl;
  bool m_validConfig;
};

/*!
 * Type alias for an unique pointer to a MPEG-H parser instance.
 */
using CUCMpeghParser = std::unique_ptr<CMpeghParser>;
}  // namespace audioparser
}  // namespace mmt
