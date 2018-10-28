#pragma once

#include <algorithm>
#include <thread>

namespace KlakNDI
{
    // NDI singleton finder class
    class Finder
    {
    public:

        // Get the singleton instance.
        static Finder& getInstance()
        {
			static Finder instance;
            return instance;
        }
        // Get one of the found sources with name matching.
        NDIlib_source_t getSourceWithNameLike(const char* name) const
        {
            // Retrieve the current source list.
            uint32_t count;
            auto sources = NDIlib_find_get_current_sources(finder_, &count);

            // Return the null source if there is no found source.
            if (count == 0) return NDIlib_source_t();

            // Return the first source if no name was given.
            if (name == nullptr || name[0] == 0) return sources[0];
            
            // Scan the source list and return the first match.
            for (uint32_t i = 0; i < count; i++)
                if (std::string(sources[i].p_ndi_name).find(name) != std::string::npos)
                    return sources[i];

            // Nothing found: Return the null source.
            return NDIlib_source_t();
        }
		void tryResetIpAddress(char* ip) {
			if (!finder_) {
				//The first time to initialize finder
				size_t length = strlen(ip) + 1;
				address_ = new char[length];
				strcpy_s(address_, length, ip);
				NDIlib_find_create_t find;
				find.p_extra_ips = address_;
				finder_ = NDIlib_find_create_v2(&find);
			}
			if (strcmp(ip, address_)!=0) {
				//When new ip comes in, use a new finder
				delete[] address_;
				size_t length = strlen(ip) + 1;
				address_ = new char[length];
				strcpy_s(address_,length, ip);
				NDIlib_find_destroy(finder_);
				NDIlib_find_create_t find;
				find.p_extra_ips = address_;
				finder_ = NDIlib_find_create_v2(&find);
			}
		}
        // Copy the names of the found sources into a given array.
        int retrieveSourceNames(const char* names[], int maxNames) const
        {
            // Retrieve the current source list.
            uint32_t count;
            auto sources = NDIlib_find_get_current_sources(finder_, &count);

            // Copy the pointers to the name strings up to maxNames.
            count = std::min(count, static_cast<uint32_t>(maxNames));
            for (uint32_t i = 0; i < count; i++) names[i] = sources[i].p_ndi_name;

            return count;
        }

    private:

        NDIlib_find_instance_t finder_;
		char* address_;

		~Finder() 
		{
			delete[] address_;
		}
        Finder()
        {
            // Create a finder instance and let it run on an independent thread.
            //NDIlib_find_create_t find;
            //finder_ = NDIlib_find_create_v2(&find);
            //std::thread(&Finder::FinderThread, this).detach();
        }

        void FinderThread()
        {
            while (true) NDIlib_find_wait_for_sources(finder_, 5000);
        }
    };
}
