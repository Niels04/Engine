#pragma once
#include <vector>

template<typename T>
class iterator
{
public:
	struct data
	{
		data() = default;
		data(const typename std::vector<T>::const_iterator iterator, const uint32_t index) : texIndex(index), it(iterator) {  }
		uint32_t texIndex = 0;
		typename std::vector<T>::const_iterator it;
	};
	iterator(const std::vector<T>& INfirst, const std::vector<T>& INsecond, uint32_t texIndexStart = 0U) {
		containers[0] = &INfirst; containers[1] = &INsecond;
		m_data.texIndex = texIndexStart;
		if (containers[0]->size())
		{
			m_data.it = containers[0]->begin();
		}
		else
		{
			m_data.it = containers[1]->begin();
			container_current++;
		}
	}
	static iterator endIterator(const std::vector<T>& container_last) { return { container_last }; }
	inline const data& operator*() const { return m_data; }
	inline bool operator !=(const iterator& rhs) {
		if (container_current == 0)
		{
			return true;
		}
		return m_data.it != rhs.m_data.it;
	}
	inline void operator ++() {//this would be an excellent use for [[likely]] and [[unlikely]]
		m_data.texIndex++;
		m_data.it++;
		if (m_data.it == containers[container_current]->end())
		{
			container_current++;
			if (container_current < 2)//only go to next container if the current_container isn't the last one
				m_data.it = containers[container_current]->begin();
		}
	}
private:
	uint8_t container_current = 0;//0 if the current container is first, 1 if the current container is second -> if there were more containers, this would be an index
	const std::vector<T>* containers[2];
	data m_data;
	iterator(const std::vector<T>& container_last) { m_data.it = container_last.end(); }
};