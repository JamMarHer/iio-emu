

#include "cf_ad9361_lpc.hpp"

#include "utils/attr_ops_xml.hpp"
#include "utils/utility.hpp"

#define AD9361_ADC_RX_CHANNELS 2
#define AD9361_ADC_RX_SAMPLE_SIZE 2
#define AD9361_ADC_RX_CHANNEL_1 0
#define AD9361_ADC_RX_CHANNEL_2 1

using namespace iio_emu;

AD9361ADCRX::AD9361ADCRX(const char* device_id, struct _xmlDoc* doc)
{
	m_device_id = device_id;
	m_doc = doc;

	m_connections = std::vector<std::pair<AbstractDeviceOut*, unsigned short>>(AD9361_ADC_RX_CHANNELS);
}

AD9361ADCRX::~AD9361ADCRX() {}

int32_t AD9361ADCRX::open_dev(size_t sample_size, uint32_t mask, bool cyclic) { return 0; }

ssize_t AD9361ADCRX::read_dev(char* pbuf, size_t offset, size_t bytes_count)
{
	std::vector<uint16_t> samples = resample(0, bytes_count);

	memcpy(pbuf, samples.data(), bytes_count);
	return bytes_count;
}

std::vector<uint16_t> AD9361ADCRX::resample(unsigned short channel, size_t len)
{
	loadValues();
	std::vector<uint16_t> samples;
	unsigned int ratio = 1E8 / m_samplerate;

	std::vector<uint16_t> tmp_samples((len / 2) * ratio);
	m_connections.at(0).first->transfer_samples_to_RX_device(reinterpret_cast<char*>(tmp_samples.data()),
								 (len / 2) * ratio);

	digital_decimation(tmp_samples, samples, ratio);

	return samples;
}

void AD9361ADCRX::loadValues()
{
	char tmp_attr[IIOD_BUFFER_SIZE];

	read_channel_attr(m_doc, "iio:device1", "voltage0", false, "sampling_frequency", tmp_attr, IIOD_BUFFER_SIZE);
	m_samplerate = safe_stod(tmp_attr);
}

void AD9361ADCRX::connectDevice(unsigned short channel_in, AbstractDeviceOut* deviceOut, unsigned short channel_out)
{
	m_connections.at(channel_in) = std::pair<AbstractDeviceOut*, unsigned short>(deviceOut, channel_out);
}

int32_t AD9361ADCRX::close_dev() { return 0; }

int32_t AD9361ADCRX::set_buffers_count(uint32_t buffers_count) { return 0; }

int32_t AD9361ADCRX::get_mask(uint32_t* mask)
{
	*mask = 3;
	return 0;
}
ssize_t AD9361ADCRX::transfer_dev_to_mem(size_t bytes_count)
{
	UNUSED(bytes_count);
	return 0;
}

int32_t AD9361ADCRX::cancel_buffer() { return 0; }
