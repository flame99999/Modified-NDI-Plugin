#pragma once

#include <unordered_map>

namespace KlakNDI
{
    // NDI receiver class
    class Receiver
    {
    public:

        Receiver(const NDIlib_source_t& source, bool lowBandWidth)
        {
			NDIlib_recv_color_format_e colorFarmat;
			if (lowBandWidth)
				colorFarmat = NDIlib_recv_color_format_fastest;
			else
				colorFarmat = NDIlib_recv_color_format_UYVY_RGBA;
			NDIlib_recv_create_v3_t create(source, colorFarmat);
			if (lowBandWidth)
				create.bandwidth = NDIlib_recv_bandwidth_lowest;
            instance_ = NDIlib_recv_create_v3(&create);
            id_ = generateID();
            getInstanceMap()[id_] = this;
			ipAddress = source.p_url_address;
        }

        ~Receiver()
        {
            NDIlib_recv_destroy(instance_);
            getInstanceMap().erase(id_);
        }

        uint32_t getID() const
        {
            return id_;
        }

        bool receiveFrame()
        {
            return NDIlib_recv_capture_v2(instance_, &frame_, nullptr, nullptr, 0) == NDIlib_frame_type_video;
        }

        void freeFrame()
        {
            NDIlib_recv_free_video_v2(instance_, &frame_);
        }

        int getFrameWidth() const
        {
            return frame_.xres;
        }

        int getFrameHeight() const
        {
            return frame_.yres;
        }

        NDIlib_FourCC_type_e getFrameFourCC() const
        {
            return frame_.FourCC;
        }

        const void* getFrameData() const
        {
            return frame_.p_data;
        }

        static Receiver* getInstanceFromID(uint32_t id)
        {
            return getInstanceMap()[id];
        }

    private:

        NDIlib_recv_instance_t instance_;
        NDIlib_video_frame_v2_t frame_;
        uint32_t id_;
	public:
		const char* ipAddress;

        static uint32_t generateID()
        {
            static uint32_t counter;
            return counter++;
        }

        static std::unordered_map<uint32_t, Receiver*>& getInstanceMap()
        {
            static std::unordered_map<uint32_t, Receiver*> map;
            return map;
        }
    };
}
