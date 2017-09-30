#pragma once
#include <string>
using namespace std;

void replace_all(std::string & s, std::string const & t, std::string const & w)
{
	string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
	while (pos != std::string::npos) { // found 
		s.replace(pos, t_size, w);
		pos = s.find(t, pos + r_size);
	}
}

/*
将char类型的时间例如 2017-09-18 18:09:25
转为char类型的时间戳 
*/
void cnDatetoMaxid(char* target, string from) {
	tm tm_;
	int year, month, day, hour, minute, second;
	sscanf(from.c_str(),"%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;

	time_t tt = mktime(&tm_);
	sprintf(target, "%d", tt);

}