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

#ifndef IIO_EMU_AD9361_DAC_TX_HPP
#define IIO_EMU_AD9361_DAC_TX_HPP

#include "iiod/devices/abstract_device_out.hpp"
struct _xmlDoc;

namespace iio_emu {

class AD9361DACTX : public AbstractDeviceOut
{
public:
	AD9361DACTX(const char* device_id, struct _xmlDoc* doc);
	~AD9361DACTX();

	int32_t open_dev(size_t sample_size, uint32_t mask, bool cyclic) override;
	int32_t close_dev() override;
	int32_t set_buffers_count(uint32_t buffers_count) override;
	int32_t get_mask(uint32_t* mask) override;
	ssize_t write_dev(const char* buf, size_t offset, size_t bytes_count) override;
	ssize_t transfer_mem_to_dev(size_t bytes_count) override;

	int32_t cancel_buffer() override;

	void transfer_samples_to_RX_device(char* buf, size_t samples_count) override;

private:
	struct _xmlDoc* m_doc;
	bool m_enable;

	std::vector<uint16_t> m_samples;
	unsigned int m_current_index;

	double m_samplerate;

	bool m_reset_buffer;

	void loadValues();
	std::vector<uint16_t> resample();
};
} // namespace iio_emu
#endif // IIO_EMU_AD9361_DAC_TX_HPP
