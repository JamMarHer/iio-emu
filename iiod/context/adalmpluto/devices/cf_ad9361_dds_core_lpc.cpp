/*
 * Copyright (c) 2021 Analog Devices Inc.
 *
 * This file is part of iio-emu
 * (see http://www.github.com/analogdevicesinc/iio-emu).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "cf_ad9361_dds_core_lpc.hpp"

#include "utils/attr_ops_xml.hpp"
#include "utils/utility.hpp"

using namespace iio_emu;

AD9361DACTX::AD9361DACTX(const char* device_id, struct _xmlDoc* doc)
{
	m_device_id = device_id;
	m_doc = doc;
	m_current_index = 0;

	m_reset_buffer = false;
}

AD9361DACTX::~AD9361DACTX() {}

int32_t AD9361DACTX::open_dev(size_t sample_size, uint32_t mask, bool cyclic) { return 0; }

int32_t AD9361DACTX::close_dev() { return 0; }

int32_t AD9361DACTX::set_buffers_count(uint32_t buffers_count) { return 0; }

int32_t AD9361DACTX::get_mask(uint32_t* mask) { return 0; }

ssize_t AD9361DACTX::write_dev(const char* buf, size_t offset, size_t bytes_count)
{
	if (m_reset_buffer) {
		m_samples.clear();
		m_current_index = 0;
		m_reset_buffer = false;
	}

	auto* samples = (uint16_t*)buf;
	for (int i = 0; i < bytes_count / 2; i++) {
		m_samples.push_back(samples[i]);
	}

	return bytes_count;
}

ssize_t AD9361DACTX::transfer_mem_to_dev(size_t bytes_count)
{
	m_samples = resample();
	m_reset_buffer = true;
	return 0;
}

std::vector<uint16_t> AD9361DACTX::resample()
{
	loadValues();
	unsigned int ratio = 1E8 / m_samplerate;

	if (ratio < 2) {
		return m_samples;
	}

	std::vector<uint16_t> samples;
	for (unsigned int i = 0; i < m_samples.size(); i++) {
		for (unsigned int j = 0; j < ratio; j++) {
			samples.push_back(m_samples.at(i));
		}
	}
	return samples;
}

void AD9361DACTX::loadValues()
{
	char tmp_attr[IIOD_BUFFER_SIZE];
	read_channel_attr(m_doc, "iio:device1", "voltage0", true, "sampling_frequency", tmp_attr, IIOD_BUFFER_SIZE);
	m_samplerate = safe_stod(tmp_attr);
}

void AD9361DACTX::transfer_samples_to_RX_device(char* buf, size_t samples_count)
{
	if (m_samples.empty()) {
		m_samples = std::vector<uint16_t>(samples_count);
	}

	std::vector<uint16_t> samples;

	for (int i = 0; i < samples_count; i++) {
		samples.push_back(m_samples.at(m_current_index));
		m_current_index++;
		if (m_current_index >= m_samples.size()) {
			m_current_index = 0;
		}
	}
	memcpy(buf, samples.data(), samples_count * sizeof(uint16_t));
}

int32_t AD9361DACTX::cancel_buffer()
{
	m_samples.clear();
	return 0;
}
