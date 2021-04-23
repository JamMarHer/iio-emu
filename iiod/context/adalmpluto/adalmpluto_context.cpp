/*
 * ADIBSD License
 *
 * Copyright (c) 2021 Analog Devices Inc.
 * All rights reserved.
 *
 * This file is part of iio-emu
 * (see http://www.github.com/analogdevicesinc/iio-emu).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *     - Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     - Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     - Neither the name of Analog Devices, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *     - The use of this software may or may not infringe the patent rights
 *       of one or more patent holders.  This license does not release you
 *       from the requirement that you obtain separate licenses from these
 *       patent holders to use this software.
 *     - Use of the software either in source or binary form, must be run
 *       on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.
 *
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
 * RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "adalmpluto_context.hpp"

#include "iiod/context/adalmpluto/devices/cf_ad9361_dds_core_lpc.hpp"
#include "iiod/context/adalmpluto/devices/cf_ad9361_lpc.hpp"
#include "utils/attr_ops_xml.hpp"
#include "utils/utility.hpp"

#include <adalmpluto_xml.h>
#include <libxml/tree.h>

using namespace iio_emu;

enum CALIBRATION_COEFFICIENT
{
	DAC_OFFSET_POS = 0,
	DAC_GAIN_POS = 1,
	ADC_OFFSET_POS = 2,
	ADC_GAIN_POS = 3,
	DAC_OFFSET_NEG = 4,
	DAC_GAIN_NEG = 5,
	ADC_OFFSET_NEG = 6,
	ADC_GAIN_NEG = 7
};

AdalmPlutoContext::AdalmPlutoContext()
	: GenericXmlContext(reinterpret_cast<const char*>(adalmpluto_xml), sizeof(adalmpluto_xml))
{
	// devices
	auto dac_tx = new AD9361DACTX("iio:device3", m_doc);
	auto adc_rx = new AD9361ADCRX("iio:device4", m_doc);

	addDevice(dac_tx);
	addDevice(adc_rx);

	assignBasicOps();

	m_ps_write_coefficients.push_back(4095.0 / (5.02 * 1.2));
	m_ps_write_coefficients.push_back(4095.0 / (-5.1 * 1.2));
	m_ps_read_coefficients.push_back(6.4 / 4095.0);
	m_ps_read_coefficients.push_back((-6.4) / 4095.0);

	m_ps_current_values = std::vector<std::string>(2);
}

AdalmPlutoContext::~AdalmPlutoContext()
{
	delete m_iiodOps;
	m_iiodOps = nullptr;

	xmlFreeDoc(m_doc);
	xmlCleanupParser();
	m_doc = nullptr;

	delete m_ctx_xml;
	m_ctx_xml = nullptr;

	for (auto dev : m_devices) {
		delete dev;
		dev = nullptr;
	}
}

ssize_t AdalmPlutoContext::chWriteAttr(const char* device_id, const char* channel, bool ch_out, const char* attr,
				       const char* buf, size_t len)
{
	auto ret = GenericXmlContext::chWriteAttr(device_id, channel, ch_out, attr, buf, len);
	return ret;
}
