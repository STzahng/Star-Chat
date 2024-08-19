#pragma once
#include "const.h"

struct SectionInfo {
	SectionInfo(){}
	~SectionInfo() { _section_datas.clear(); }

	std::map<std::string, std::string>_section_datas;
	SectionInfo(const SectionInfo& src) {
		_section_datas = src._section_datas;
	}
	SectionInfo& operator = (const SectionInfo& src) {
		if (&src == this) {
			return *this;
		}
		this->_section_datas = src._section_datas;
	}
	std::string operator[](const std::string& key) {
		if (_section_datas.find(key) == _section_datas.end()) {
			return "";
		}
		return _section_datas[key];
	}
};

class ConfigMgr
{
public:
	~ConfigMgr() { _config_map.clear(); }
	SectionInfo operator[](const std::string& section);

	static ConfigMgr& Inst();
	ConfigMgr(const ConfigMgr& src);
	ConfigMgr& operator=(const ConfigMgr& src);
private:
	ConfigMgr();
	std::map<std::string, SectionInfo>_config_map;
};

