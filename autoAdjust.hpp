#pragma once
#include "jconf.h"
#include "console.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

int32_t get_masked(int32_t val, int32_t h, int32_t l)
{
	val &= (0x7FFFFFFF >> (31-(h-l))) << l;
	return val >> l;
}

class autoAdjust
{
public:

	autoAdjust()
	{
	}

	void printConfig()
	{
		printer::inst()->print_str("The configuration for 'cpu_threads_conf' in your config file is 'null'.\n");
		printer::inst()->print_str("The miner evaluates your system and prints a suggestion for the section `cpu_threads_conf` to the terminal.\n");
		printer::inst()->print_str("The values are not optimal, please try to tweak the values based on notes in config.txt.\n");
		printer::inst()->print_str("Please copy & paste the block within the asterisks to your config.\n\n");

		int32_t L3KB_size = 0;

		if(!detectL3Size(L3KB_size))
			return;

		if(L3KB_size < 1024 || L3KB_size > 102400)
		{
			printer::inst()->print_msg(L0, "Autoconf failed: L3 size sanity check failed %u.", L3KB_size);
			return;
		}

		printer::inst()->print_msg(L0, "Autoconf L3 size detected at %u.", L3KB_size);

		uint32_t corecnt;
		bool linux_layout;

		detectCPUConf(corecnt, linux_layout);

		printer::inst()->print_msg(L0, "Autoconf cores detected at %u on %s.", corecnt,
			linux_layout ? "Linux" : "Windows");

		printer::inst()->print_str("\n**************** Copy&Paste ****************\n\n");
		printer::inst()->print_str("\"cpu_threads_conf\" :\n[\n");

		uint32_t aff_id = 0;
		char strbuf[256];
		for(uint32_t i=0; i < corecnt; i++)
		{
			bool double_mode;

			if(L3KB_size < 0)
				break;

			double_mode = L3KB_size / 2048 > (int32_t)(corecnt-i);

			snprintf(strbuf, sizeof(strbuf), "   { \"low_power_mode\" : %s, \"no_prefetch\" : true, \"affine_to_cpu\" : %u },\n",
				double_mode ? "true" : "false", aff_id);
			printer::inst()->print_str(strbuf);

			if(linux_layout)
				aff_id++;
			else
				aff_id += 2;

			if(double_mode)
				L3KB_size -= 4096;
			else
				L3KB_size -= 2048;
		}

		printer::inst()->print_str("]\n\n**************** Copy&Paste ****************\n");
	}

private:
	bool detectL3Size(int32_t &l3kb)
	{
		int32_t cpu_info[4];
		char cpustr[13] = {0};

		jconf::cpuid(0, 0, cpu_info);
		memcpy(cpustr, &cpu_info[1], 4);
		memcpy(cpustr+4, &cpu_info[3], 4);
		memcpy(cpustr+8, &cpu_info[2], 4);

		if(strcmp(cpustr, "GenuineIntel") == 0)
		{
			jconf::cpuid(4, 3, cpu_info);

			if(get_masked(cpu_info[0], 7, 5) != 3)
			{
				printer::inst()->print_msg(L0, "Autoconf failed: Couln't find L3 cache page.");
				return false;
			}

			l3kb = ((get_masked(cpu_info[1], 31, 22) + 1) * (get_masked(cpu_info[1], 21, 12) + 1) *
				(get_masked(cpu_info[1], 11, 0) + 1) * (cpu_info[2] + 1)) / 1024;

			return true;
		}
		else if(strcmp(cpustr, "AuthenticAMD") == 0)
		{
			jconf::cpuid(0x80000006, 0, cpu_info);

			l3kb = get_masked(cpu_info[3], 31, 18) * 512;

			return true;
		}
		else
		{
			printer::inst()->print_msg(L0, "Autoconf failed: Unknown CPU type: %s.", cpustr);
			return false;
		}
	}

	void detectCPUConf(uint32_t &corecnt, bool &linux_layout)
	{
#ifdef _WIN32
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		corecnt = info.dwNumberOfProcessors;
		linux_layout = false;
#else
		corecnt = sysconf(_SC_NPROCESSORS_ONLN);
		linux_layout = true;
#endif // _WIN32
	}
};
